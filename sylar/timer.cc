#include "timer.h"
#include "util.h"
#include "log.h"

namespace sylar {

bool Timer::Comparator::operator()(const Timer::ptr& lhs,
                                   const Timer::ptr& rhs) const {
    if(!lhs && !lhs) {
        return false;
    }
    if(!lhs) {
        return true;
    }
    if(!rhs) {
        return false;
    }
    if(lhs->m_next < rhs->m_next) {
        return true;
    }
    if(lhs->m_next > rhs->m_next) {
        return false;
    }
    return lhs.get() < rhs.get();
}

Timer::Timer(uint64_t ms, std::function<void()> cb,
             bool recurring, TimerManager* manager)
      :m_recurring(recurring)
      ,m_ms(ms)
      ,m_cb(cb)
      ,m_manager(manager) {
    m_next = sylar::GetCurrentMS() + m_ms;
    SYLAR_LOG_DEBUG(SYLAR_LOG_NAME("system")) << "Timer constract next_time: " << m_next
        << ", ms : " << m_ms
        << ", now_time : " << sylar::GetCurrentMS();
}

Timer::Timer(uint64_t next)
    :m_next(next) {
}

bool Timer::cancel() {
    TimerManager::RWMutexType::WriteLock lock(m_manager->m_mutex);
    if (m_cb) {
        m_cb = nullptr;
        auto it = m_manager->m_timers.find(shared_from_this());
        m_manager->m_timers.erase(it);
        return true;
    }
    return false;
}

bool Timer::refresh() {
    TimerManager::RWMutexType::WriteLock lock(m_manager->m_mutex);
    if (!m_cb) {
        return false;
    }
    
    auto it = m_manager->m_timers.find(shared_from_this());
    if (it == m_manager->m_timers.end()) {
        return false;
    }

    m_manager->m_timers.erase(it);
    m_next = sylar::GetCurrentMS() + m_ms;
    m_manager->m_timers.insert(shared_from_this());
    return true;
}

bool Timer::reset(uint64_t ms, bool from_now) {
    if (m_ms == ms && !from_now) {
        return true;
    }
    TimerManager::RWMutexType::WriteLock lock(m_manager->m_mutex);
    if (!m_cb) {
        return false;
    }
    auto it = m_manager->m_timers.find(shared_from_this());
    if (it == m_manager->m_timers.end()) {
        return false;
    }

    m_manager->m_timers.erase(it);
    uint64_t start = 0;
    if (from_now) {
        start = sylar::GetCurrentMS();
    } else {
        start = m_next - m_ms;
    }
    m_ms = ms;
    m_next = m_ms + start;
    m_manager->addTimer(shared_from_this(), lock);
    return true;
}

TimerManager::TimerManager() {
    m_previouseTime = sylar::GetCurrentMS();
}

TimerManager::~TimerManager() {

}

Timer::ptr TimerManager::addTimer(uint64_t ms, std::function<void()> cb,
                                    bool recurring) {
    // Timer::ptr timer = std::make_shared<Timer>(ms, cb, recurring, this);     make_shared<> 不能调用私有构造函数
    Timer::ptr timer(new Timer(ms, cb, recurring, this));
    RWMutexType::WriteLock lock(m_mutex);
    addTimer(timer, lock);
    return timer;
}

static void OnTimer(std::weak_ptr<void> weak_cond, std::function<void()> cb) {
    std::shared_ptr<void> tmp = weak_cond.lock();
    if (tmp) {
        cb();
    }
}

Timer::ptr TimerManager::addConditionTimer(uint64_t ms, std::function<void()> cb,
                                        std::weak_ptr<void> weak_cond,
                                        bool recurring) {
    return addTimer(ms, std::bind(&OnTimer, weak_cond, cb), recurring);   
}

uint64_t TimerManager::getNextTimer() {
    RWMutexType::ReadLock lock(m_mutex);
    m_tickled = false;
    if (m_timers.empty()) {
        return ~0ull;
    }
    const Timer::ptr& next = *m_timers.begin();
    uint64_t now_ms = sylar::GetCurrentMS();
    if (now_ms >= next->m_next) {
        return 0;
    } else {
        return next->m_next - now_ms;
    }
}

void TimerManager::listExpiredCb(std::vector<std::function<void()> >& cbs) {
    uint64_t now_ms = sylar::GetCurrentMS();
    std::vector<Timer::ptr> expired;
    {
        RWMutexType::ReadLock lock(m_mutex);
        if (m_timers.empty()) {
            return;
        }
    }
    RWMutexType::WriteLock lock(m_mutex);
    if (m_timers.empty()) {
        return;
    }
    bool rollover = detectClockRollover(now_ms);
    if (!rollover && ((*m_timers.begin())->m_next > now_ms)) {
        return;
    }

    // Timer::ptr now_timer = std::make_shared<Timer>(now_ms);   make_shared<> 不能调用私有构造函数
    Timer::ptr now_timer(new Timer(now_ms));
    // 若系统时间改动则将m_timers的所有Timer视为过期的, 否则返回第一个 >= now_ms的迭代器
    auto it = rollover ? m_timers.end() : m_timers.lower_bound(now_timer);
    // 筛选出当前时间等于next时间执行的Timer
    while (it != m_timers.end() && (*it)->m_next == now_ms) {
        ++it;
    }
    // 小于当前时间的Timer都是已经过了时间的Timer，it是当前时间要执行的Timer
    // 将这些Timer都加入到expired中
    expired.insert(expired.begin(), m_timers.begin(), it);
    m_timers.erase(m_timers.begin(), it);
    cbs.reserve(expired.size());

    for (auto& timer : expired) {
        cbs.push_back(timer->m_cb);
        if (timer->m_recurring) {
            timer->m_next = now_ms + timer->m_ms;
            m_timers.insert(timer);
        } else {
            timer->m_cb = nullptr;
        }
    }
}

void TimerManager::addTimer(Timer::ptr val, RWMutexType::WriteLock& lock) {
    auto it = m_timers.insert(val).first; 
    bool at_front = (it == m_timers.begin() && !m_tickled);
    if (at_front) {
        m_tickled = true;
    }
    lock.unlock();

    if (at_front) {
        onTimerInsertedAtFront();
    }
}

bool TimerManager::hasTimer() {
    RWMutexType::ReadLock lock(m_mutex);
    return m_timers.empty();
}

bool TimerManager::detectClockRollover(uint64_t now_ms) {
    bool rollover = false;
    if (now_ms < m_previouseTime &&
        now_ms < (m_previouseTime - 60 * 60 * 1000)) {
        rollover = true;
    }

    m_previouseTime = now_ms;
    return rollover;
}

}