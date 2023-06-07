#include <sys/epoll.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

#include "iomanager.h"
#include "macro.h"

namespace sylar {

static SYLAR__SYSTEM__LOG(g_logger);

IOManager::FdContext::EventContext& IOManager::FdContext::getContext(Event event) {
    switch (event) {
        case IOManager::READ:
            return read;
        case IOManager::WRITE:
            return write;
        default:
            SYLAR_ASSERT2(false, "getContext");
    }
}

void IOManager::FdContext::resetContext(EventContext& ctx) {
    ctx.scheduler = nullptr;
    ctx.fiber.reset();
    ctx.cb = nullptr;
}

void IOManager::FdContext::triggerEvent(Event event) {
    SYLAR_ASSERT(events & event);
    events = (Event)(events & ~event);
    EventContext &ctx = getContext(event);
    if (ctx.cb) {
        ctx.scheduler->schedule(&ctx.cb);
    } else {
        ctx.scheduler->schedule(&ctx.fiber);
    }
    ctx.scheduler = nullptr;
    return;
}

IOManager::IOManager(size_t threads, bool use_caller, const std::string &name)
    :Scheduler(threads, use_caller, name) {
    m_epfd = epoll_create(5000);
    // �ɹ�ʱ����Щϵͳ���ý����طǸ��ļ�����������������򷵻�-1�����ҽ�errno����Ϊָʾ����
    SYLAR_ASSERT(m_epfd != -1);

    // �ɹ�����0��ʧ�ܷ���-1����������errno��
    int rt = pipe(m_tickleFds);
    SYLAR_ASSERT(!rt);

    epoll_event event;
    // ��0��ʼ��event
    memset(&event, 0, sizeof(epoll_event));
    event.events = EPOLLIN | EPOLLET;
    event.data.fd = m_tickleFds[0];

    // ��һ���򿪵��ļ�������ִ��һϵ�п��Ʋ���
    // F_SETFL: ��ȡ/�����ļ�״̬��־
    // O_NONBLOCK: ʹI/O��ɷ�����ģʽ���ڶ�ȡ�������ݻ���д�뻺��������������return�������������ȴ���
    rt = fcntl(m_tickleFds[0], F_SETFL, O_NONBLOCK);
    SYLAR_ASSERT(!rt);

    rt = epoll_ctl(m_epfd, EPOLL_CTL_ADD, m_tickleFds[0], &event);
    SYLAR_ASSERT(!rt);

    contextResize(32);

    start();
}

IOManager::~IOManager() {
    stop();
    close(m_epfd);
    close(m_tickleFds[0]);
    close(m_tickleFds[1]);

    for (size_t i = 0; i < m_fdContexts.size(); ++i) {
        if (m_fdContexts[i]) {
            delete m_fdContexts[i];
        }
    }
}

void IOManager::contextResize(size_t size) {
    m_fdContexts.resize(size);

    for (size_t i = 0; i < m_fdContexts.size(); ++i) {
        if (!m_fdContexts[i]) {
            m_fdContexts[i] = new FdContext;
            m_fdContexts[i]->fd = i;
        }
    }
}

 // 0: success, -1: error
int IOManager::addEvent(int fd, Event event, std::function<void()> cb) {
    FdContext* fd_ctx = nullptr;
    RWMutexType::ReadLock lock(m_mutex);
    if ((int)m_fdContexts.size() > fd) {
        fd_ctx = m_fdContexts[fd];
        lock.unlock();
    } else {
        lock.unlock();
        RWMutexType::WriteLock lock2(m_mutex);
        contextResize(fd * 1.5);
        fd_ctx = m_fdContexts[fd];
    }

    FdContext::MutexType::Lock lock2(fd_ctx->mutex);
    // һ�����һ�㲻���ظ���ͬһ���¼��� ������������ͬ���߳��ڲٿ�ͬһ���������¼�
    if (fd_ctx->events & event) {
        SYLAR_LOG_ERROR(g_logger) << "addEvent assert fd = " << fd
                                  << ", event = " << event
                                  << ", fd_ctx.event = " << fd_ctx->events;
        SYLAR_ASSERT(!(fd_ctx->events & event));
    }

    // EPOLL_CTL_MOD: �޸ļ����б����Ѿ����ڵ���������������fd�����޸�����ӵ��¼����ͣ�����event����
    int op = fd_ctx->events ? EPOLL_CTL_MOD : EPOLL_CTL_ADD;
    epoll_event epevent;
    epevent.events = EPOLLET | fd_ctx->events | event;
    epevent.data.ptr = fd_ctx;

    int rt = epoll_ctl(m_epfd, op, fd, &epevent);
    if (rt) {
        SYLAR_LOG_ERROR(g_logger) << "epoll_ctl(" << m_epfd << ","
                                  << op << ", " << fd << ", " << epevent.events << ") :"
                                  << rt << " (" << errno << ") (" << strerror(errno) << ")";
        return -1;
    }

    ++m_pendingEventCount;
    fd_ctx->events = (Event)(fd_ctx->events | event);
    FdContext::EventContext &event_ctx = fd_ctx->getContext(event);
    SYLAR_ASSERT(!event_ctx.scheduler
                    && !event_ctx.fiber
                    && !event_ctx.cb);
    event_ctx.scheduler = Scheduler::GetThis();
    if (cb) {
        event_ctx.cb.swap(cb);
    } else {
        event_ctx.fiber = Fiber::GetThis();
        SYLAR_ASSERT(event_ctx.fiber->getState() == Fiber::EXEC);
    }
    return 0;
}

bool IOManager::delEvent(int fd, Event event) {
    RWMutexType::ReadLock lock(m_mutex);
    if ((int)m_fdContexts.size() <= fd) {
        return false;
    }
    FdContext* fd_ctx = m_fdContexts[fd];
    lock.unlock();

    FdContext::MutexType::Lock lock2(fd_ctx->mutex);
    if (!(fd_ctx->events & event)) {
        return false;
    }

    Event new_events = (Event)(fd_ctx->events & ~event);
    int op = new_events ? EPOLL_CTL_MOD : EPOLL_CTL_DEL;
    epoll_event epevent;
    epevent.events = EPOLLET | new_events;
    epevent.data.ptr = fd_ctx;

    int rt = epoll_ctl(m_epfd, op, fd, &epevent);
    if (rt) {
        SYLAR_LOG_ERROR(g_logger) << "epoll_ctl(" << m_epfd << ","
                                  << op << ", " << fd << ", " << epevent.events << ") :"
                                  << rt << " (" << errno << ") (" << strerror(errno) << ")";
        return false;
    }

    --m_pendingEventCount;
    fd_ctx->events = new_events;
    FdContext::EventContext& event_ctx = fd_ctx->getContext(event);
    fd_ctx->resetContext(event_ctx);
    return true;
}

bool IOManager::cancelEvent(int fd, Event event) {
    RWMutexType::ReadLock lock(m_mutex);
    if ((int)m_fdContexts.size() <= fd) {
        return false;
    }
    FdContext* fd_ctx = m_fdContexts[fd];
    lock.unlock();

    FdContext::MutexType::Lock lock2(fd_ctx->mutex);
    if (!(fd_ctx->events & event)) {
        return false;
    }

    Event new_events = (Event)(fd_ctx->events & ~event);
    int op = new_events ? EPOLL_CTL_MOD : EPOLL_CTL_DEL;
    epoll_event epevent;
    epevent.events = EPOLLET | new_events;
    epevent.data.ptr = fd_ctx;

    int rt = epoll_ctl(m_epfd, op, fd, &epevent);
    if (rt) {
        SYLAR_LOG_ERROR(g_logger) << "epoll_ctl(" << m_epfd << ","
                                  << op << ", " << fd << ", " << epevent.events << ") :"
                                  << rt << " (" << errno << ") (" << strerror(errno) << ")";
        return false;
    }

    fd_ctx->triggerEvent(event);
    --m_pendingEventCount;

    return true;
}

// ȡ����������¼�
bool IOManager::cancelAll(int fd) {
    RWMutexType::ReadLock lock(m_mutex);
    if ((int)m_fdContexts.size() <= fd) {
        return false;
    }
    FdContext* fd_ctx = m_fdContexts[fd];
    lock.unlock();

    FdContext::MutexType::Lock lock2(fd_ctx->mutex);
    if (!fd_ctx->events) {
        return false;
    }

    int op = EPOLL_CTL_DEL;
    epoll_event epevent;
    epevent.events = 0;
    epevent.data.ptr = fd_ctx;

    int rt = epoll_ctl(m_epfd, op, fd, &epevent);
    if (rt) {
        SYLAR_LOG_ERROR(g_logger) << "epoll_ctl(" << m_epfd << ","
                                  << op << ", " << fd << ", " << epevent.events << ") :"
                                  << rt << " (" << errno << ") (" << strerror(errno) << ")";
        return false;
    }
    
    if (fd_ctx->events & READ) {
        fd_ctx->triggerEvent(READ);
        --m_pendingEventCount;
    }

    if (fd_ctx->events & WRITE) {
        fd_ctx->triggerEvent(WRITE);
        --m_pendingEventCount;
    }

    SYLAR_ASSERT(fd_ctx->events == 0);
    return true;
}

IOManager* IOManager::GetThis() {
    return dynamic_cast<IOManager *>(Scheduler::GetThis());
}

void IOManager::tickle() {
    // SYLAR_LOG_DEBUG(g_logger) << "tickle()";
    if (!hasIdleThread()) {
        return;
    }
    // SYLAR_LOG_DEBUG(g_logger) << "tickle() success";
    int rt = write(m_tickleFds[1], "T", 1);
    SYLAR_ASSERT(rt == 1);
}

bool IOManager::stopping(uint64_t& timeout) {
    // SYLAR_LOG_DEBUG(g_logger) << "IOMANAGER STOPPING";
    timeout = getNextTimer();
    return timeout == ~0ull
        && m_pendingEventCount == 0
        && Scheduler::stopping();
}

bool IOManager::stopping() {
    uint64_t timeout = 0;
    return stopping(timeout);
}

// bool IOManager::stopping() {
//     uint64_t timeout = getNextTimer();
//     return timeout == ~0ull
//         && m_pendingEventCount == 0
//         && Scheduler::stopping();
// }

void IOManager::idle() {
    SYLAR_LOG_DEBUG(g_logger) << "idle()";
    epoll_event* events = new epoll_event[64]();
    // ʹ������ָ���й�events�� �뿪idle�Զ��ͷ�
    std::shared_ptr<epoll_event> shared_events(events, [](epoll_event *ptr)
                                              { delete[] ptr; });

    while (true) { 
        uint64_t next_timeout = 0;
        if (stopping(next_timeout)) {
            SYLAR_LOG_INFO(g_logger) << sylar::Thread::GetName()
                                     << " is done";
            break;
        }
        
        int rt = 0;
        do {
            // ���뼶����
            static const int MAX_TIMEOUT = 3000;
            if (next_timeout != ~0ull) {
                next_timeout = (int)next_timeout > MAX_TIMEOUT
                                ? MAX_TIMEOUT : (int)next_timeout;
            } else {
                next_timeout = MAX_TIMEOUT;
            }
            rt = epoll_wait(m_epfd, events, 64, (int)next_timeout);
            // SYLAR_LOG_DEBUG(g_logger) << "epoll wait rt = " << rt;
            // SYLAR_LOG_DEBUG(g_logger) << "next_timeout = " << next_timeout;
            // ��ʱ���������ݻ���������ϵͳҲ���п���ʹerrnoΪEINTR
            // ����ϵͳ�����ݻ���֮ǰǿ���ж��ˣ�����һ�´ε�epoll_wait
            if(rt < 0 && errno == EINTR) {
            } else {
                break;
            }
        } while (true);
        std::vector<std::function<void()> > cbs;

        listExpiredCb(cbs);
        // SYLAR_LOG_DEBUG(g_logger) << "listExpiredCb : " << cbs.size();

        if (!cbs.empty()) {
            Scheduler::schedule(cbs.begin(), cbs.end());
            cbs.clear();
        }

        for (int i = 0; i < rt; ++i) {
            epoll_event &event = events[i];
            if (event.data.fd == m_tickleFds[0]) {
                uint8_t dummy;
                while (read(m_tickleFds[0], &dummy, 1) == 1);
                continue;
            }

            FdContext *fd_ctx = (FdContext*)event.data.ptr;
            FdContext::MutexType::Lock lock(fd_ctx->mutex);
            // EPOLLERR: ��ʾ��Ӧ�����ӷ�������
            // EPOLLHUP: ��ʾ��Ӧ�����ӱ�����
            if (event.events & (EPOLLERR | EPOLLHUP)) {
                event.events |= (EPOLLIN | EPOLLOUT) & fd_ctx->events;
            }
            int real_events = NONE;
            if (event.events & EPOLLIN) {
                real_events |= READ;
            }
            if (event.events & EPOLLOUT) {
                real_events |= WRITE;
            }

            // û�¼�
            if ((fd_ctx->events & real_events) == NONE) {
                continue;
            }

            // ʣ���¼�
            int left_events = (fd_ctx->events & ~real_events);
            int op = left_events ? EPOLL_CTL_MOD : EPOLL_CTL_DEL;
            event.events = EPOLLET | left_events;

            int rt2 = epoll_ctl(m_epfd, op, fd_ctx->fd, &event);
            if (rt2) {
                SYLAR_LOG_ERROR(g_logger) << "epoll_ctl(" << m_epfd << ","
                                          << op << ", " << fd_ctx->fd << ", " << event.events << ") :"
                                          << rt2 << " (" << errno << ") (" << strerror(errno) << ")";
                continue;
            }

            if (real_events & READ) {
                fd_ctx->triggerEvent(READ);
                --m_pendingEventCount;
            }
            if (real_events & WRITE) {
                fd_ctx->triggerEvent(WRITE);
                --m_pendingEventCount;
            }
        }

        Fiber::ptr cur = Fiber::GetThis();
        auto raw_ptr = cur.get();
        cur.reset();

        // ִ���귵��scheduler��MainFiber  
        // scheduler.cc:240  idle_fiber->swapIn();
        raw_ptr->swapOut();
    }
}

void IOManager::onTimerInsertedAtFront() {
    tickle();
}

}