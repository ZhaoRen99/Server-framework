#include "scheduler.h"
#include "log.h"
#include "macro.h"

namespace sylar {

static SYLAR__SYSTEM__LOG(g_logger);

// 当前协程调度器
static thread_local Scheduler* t_secheduler = nullptr;
// 主协程
static thread_local Fiber* t_fiber = nullptr;

Scheduler::Scheduler(size_t threads, bool use_caller, const std::string &name)
    :m_name(name) {
    SYLAR_ASSERT(threads > 0);

    if (use_caller) {
        sylar::Thread::SetName(m_name);
        sylar::Fiber::GetThis();
        --threads;

        SYLAR_ASSERT(GetThis() == nullptr);
        t_secheduler = this;

        // m_rootFiber.reset(new Fiber(&Scheduler::run, 0, true));
        // 非静态成员函数需要传递this指针作为第一个参数，用 std::bind()进行绑定
        m_rootFiber.reset(new Fiber(std::bind(&Scheduler::run, this), 0, true));

        t_fiber = m_rootFiber.get();
        // SYLAR_LOG_DEBUG(g_logger) << "t_fiber" << t_fiber;
        // SYLAR_LOG_DEBUG(g_logger) << "t_fiber id" << t_fiber->getId();

        m_rootThread = sylar::GetThreadId();
        m_threadIds.push_back(m_rootThread);
    }
    else {
        m_rootThread = -1;
    }
    m_threadCount = threads;
}

Scheduler::~Scheduler() {
    SYLAR_ASSERT(m_stopping);
    if (GetThis() == this) {
        t_secheduler = nullptr;
    }
}

Scheduler* Scheduler::GetThis() {
    return t_secheduler;
}

Fiber* Scheduler::GetMainFiber() {
    return t_fiber;
}

void Scheduler::start() {
    // SYLAR_LOG_DEBUG(g_logger) << "start()";
    MutexType::Lock lock(m_mutex);
    // 已经启动了
    if (!m_stopping) {
        return;
    }
    m_stopping = false;

    SYLAR_ASSERT(m_threads.empty());

    m_threads.resize(m_threadCount);
    for (size_t i = 0; i < m_threadCount; ++i) {
        m_threads[i].reset(new Thread(std::bind(&Scheduler::run, this)
                        , m_name + "_" + std::to_string(i)));
        m_threadIds.push_back(m_threads[i]->getId());
    }
    lock.unlock();
    
    // if(m_rootFiber) {
    //     // t_fiber = m_rootFiber.get(), 从自己切换到自己了属于是
    //     // m_rootFiber->swapIn();
    //
    //     m_rootFiber->call();
    // }

    // SYLAR_LOG_DEBUG(g_logger) << "start() end";
}

void Scheduler::stop() {
    // SYLAR_LOG_DEBUG(g_logger) << "stop()";
    m_autoStop = true;
    if (m_rootFiber 
                && m_threadCount == 0
                && (m_rootFiber->getState() == Fiber::TERM
                    || m_rootFiber->getState() == Fiber::INIT)) {
        SYLAR_LOG_INFO(g_logger) << this->m_name << " sheduler stopped";
        m_stopping = true;

        if (stopping()) {
            return;
        }
    }

    // bool exit_on_this_fiber = false;
    // use_caller线程
    if (m_rootThread != -1) {
        SYLAR_ASSERT(GetThis() == this);
    }
    else {
        SYLAR_ASSERT(GetThis() != this);
    }

    m_stopping = true;
    for (size_t i = 0; i < m_threadCount; ++i) {
        tickle();
    }

    if (m_rootFiber) {
        tickle();
    }

    if (m_rootFiber) {
        // while (!stopping()) {
        //     if (m_rootFiber->getState() == Fiber::TERM
        //                 || m_rootFiber->getState() == Fiber::EXCEPT) {
        //         m_rootFiber.reset(new Fiber(std::bind(&Scheduler::run, this), 0, true));
        //         SYLAR_LOG_INFO(g_logger) << "root fiber is term, reset";
        //         t_fiber = m_rootFiber.get();
        //     }
        //     m_rootFiber->call();
        // }
        if (!stopping()) {
            m_rootFiber->call();
        }
    }

    std::vector<Thread::ptr> thrs;
    {
        MutexType::Lock lock(m_mutex);
        thrs.swap(m_threads);
    }

    for (auto& i : thrs) {
        i->join();
    }
    // if (exit_on_this_fiber) {
    // }
}

void Scheduler::setThis() {
    t_secheduler = this;
}

void Scheduler::run() {
    SYLAR_LOG_DEBUG(g_logger) << "run()";
    set_hook_enable(true);
    setThis();

    // SYLAR_LOG_DEBUG(g_logger) << sylar::GetThreadId();
    // SYLAR_LOG_DEBUG(g_logger) << t_fiber->GetFiberId();

    if (sylar::GetThreadId() != m_rootThread) {
        t_fiber = Fiber::GetThis().get();
    }

    Fiber::ptr idle_fiber(new Fiber(std::bind(&Scheduler::idle, this)));
    // SYLAR_LOG_DEBUG(g_logger) << "new idle_fiber :" << idle_fiber->getId();
    Fiber::ptr cb_fiber;

    FiberAndThread ft;
    while (true) {
        ft.reset();
        bool tickle_me = false;
        bool is_active = false;
        {
            MutexType::Lock lock(m_mutex);
            auto it = m_fibers.begin();
            while (it != m_fibers.end()) {
                if (it->thread != -1 && it->thread != sylar::GetThreadId()) {
                    ++it;
                    tickle_me = true;
                    continue;
                }
                SYLAR_ASSERT(it->fiber || it->cb);
                if (it->fiber && it->fiber->getState() == Fiber::EXEC) {
                    ++it;
                    continue;
                }

                ft = *it;
                m_fibers.erase(it);
                ++m_activateThreadCount;
                is_active = true;
                break;
            }
        }
        if (tickle_me) {
            tickle();
        }

        if (ft.fiber && (ft.fiber->getState() != Fiber::TERM
                            || ft.fiber->getState() != Fiber::EXCEPT)) {
            ft.fiber->swapIn();
            --m_activateThreadCount;

            if (ft.fiber->getState() == Fiber::READY) {
                schedule(ft.fiber);
            } else if (ft.fiber->getState() != Fiber::TERM
                            && ft.fiber->getState() != Fiber::EXCEPT) {
                ft.fiber->setState(Fiber::HOLD);
            }
            ft.reset();
        } else if(ft.cb) {
            if (cb_fiber) {
                cb_fiber->reset(ft.cb);
            } else {
                cb_fiber.reset(new Fiber(ft.cb));
                // SYLAR_LOG_DEBUG(g_logger) << "new ft.cb: " << cb_fiber->getId();
            }
            ft.reset();
            cb_fiber->swapIn();
            --m_activateThreadCount;
            if (cb_fiber->getState() == Fiber::READY) {
                schedule(cb_fiber);
                cb_fiber.reset();
            } else if (cb_fiber->getState() == Fiber::EXCEPT
                        || cb_fiber->getState() == Fiber::TERM) {
                cb_fiber->reset(nullptr);
            } else {
                cb_fiber->setState(Fiber::HOLD);
                cb_fiber.reset();
            }
        } else {
            if (is_active) {
                --m_activateThreadCount;
                continue;
            }
            if (idle_fiber->getState() == Fiber::TERM) {
                SYLAR_LOG_DEBUG(g_logger) << "idle_fiber term"; 
                break;
            }

            ++m_idleThreadCount;
            idle_fiber->swapIn();
            --m_idleThreadCount;
            if (idle_fiber->getState() != Fiber::TERM
                    && idle_fiber->getState() != Fiber::EXCEPT) {
                idle_fiber->setState(Fiber::HOLD);
            }
        }
    }
}

void Scheduler::tickle() {
    SYLAR_LOG_DEBUG(g_logger) << "tickle()";
}

bool Scheduler::stopping() {
    MutexType::Lock lock(m_mutex);
    // SYLAR_LOG_INFO(g_logger) << "stopping()";

    // SYLAR_LOG_DEBUG(g_logger) << boost::lexical_cast<std::string>(m_autoStop)
    //                           << boost::lexical_cast<std::string>(m_stopping)
    //                           << boost::lexical_cast<std::string>(m_fibers.empty())
    //                           << boost::lexical_cast<std::string>(m_activateThreadCount == 0);

    return m_autoStop && m_stopping 
        && m_fibers.empty() && m_activateThreadCount == 0;
}

void Scheduler::idle() {
    SYLAR_LOG_DEBUG(g_logger) << "idle()";
    while (!stopping()) {
        sylar::Fiber::YieldToHold();
    }
}
}