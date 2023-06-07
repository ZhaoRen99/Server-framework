#ifndef __SYLAR_IOMANAGER_H__
#define __SYLAR_IOMANAGER_H__

#include "scheduler.h"
#include "timer.h"

namespace sylar {

class IOManager : public Scheduler, public TimerManager {
public:
    typedef std::shared_ptr<IOManager> ptr;
    typedef RWMutex RWMutexType;

    enum Event
    {
        // 无事件
        NONE    = 0x0,
        // 读事件(EPOLLIN)
        READ    = 0x1,
        // 写事件(EPOLLOUT)
        WRITE   = 0x4,

    };

private:
    /**
     * @brief socket事件上下文
     * 
     */
    struct FdContext {
        typedef Mutex MutexType;
        /**
         * @brief 事件上下文
         * 
         */
        struct EventContext {
            Scheduler* scheduler = nullptr; //事件执行的scheduler
            Fiber::ptr fiber;               //事件协程
            std::function<void()> cb;       //事件执行的回调函数
        };

        /**
         * @brief 获取对应事件上下文
         * 
         * @param event 
         * @return EventContext& 
         */
        EventContext& getContext(Event event);

        /**
         * @brief 重置事件上下文
         * 
         * @param ctx 
         */
        void resetContext(EventContext& ctx);

        /**
         * @brief 触发事件
         * 
         * @param event 
         */
        void triggerEvent(Event event);

        EventContext read;      //读事件
        EventContext write;     //写事件
        int fd;                 //事件关联的句柄
        Event events = NONE;    //已经注册的事件
        MutexType mutex;        
    };

public:
    /**
     * @brief 构造函数
     * 
     * @param threads   线程数量 
     * @param use_caller    是否将调用线程包含进去
     * @param name  调度器名称
     */
    IOManager(size_t threads = 1, bool use_caller = true, const std::string &name = "main");
    
    /**
     * @brief 析构函数
     * 
     */
    ~IOManager();

    /**
     * @brief 添加事件
     * 
     * @param fd socket句柄
     * @param event 事件类型
     * @param cb 事件回调函数
     * @return int 成功返回0， 失败返回-1
     */
    int addEvent(int fd, Event event, std::function<void()> cb = nullptr);

    /**
     * @brief 删除事件
     * 
     * @param fd socket句柄
     * @param event 事件类型
     * @return true 
     * @return false 
     * @attention 不会触发事件
     */
    bool delEvent(int fd, Event event);

    /**
     * @brief 取消时间
     * 
     * @param fd socket句柄
     * @param event 事件类型
     * @return true 
     * @return false 
     * @attention 如果事件存在则触发事件
     */
    bool cancelEvent(int fd, Event event);

    /**
     * @brief 取消所有事件
     * 
     * @param fd socket句柄
     * @return true 
     * @return false 
     */
    bool cancelAll(int fd);

    /**
     * @brief 获得当前IOManager
     * 
     * @return IOManager* 
     */
    static IOManager* GetThis();

protected:
    void tickle() override;
    bool stopping() override;
    void idle() override;
    void onTimerInsertedAtFront() override;

    /**
     * @brief 重置socket句柄上下文的容器大小
     * 
     * @param size 容器大小
     */
    void contextResize(size_t size);

    /**
     * @brief 判断是否可以停止
     * 
     * @param timeout 最近要出发的定时器事件间隔
     * @return true 
     * @return false 
     */
    bool stopping(uint64_t& timeout);

private:
    //epoll文件句柄
    int m_epfd = 0; 
    // pipe文件句柄，其中fd[0]表示读端，fd[1] 表示写端
    int m_tickleFds[2];
    // 等待执行的事件数量
    std::atomic<size_t> m_pendingEventCount = {0}; 
    RWMutexType m_mutex;
    //socket事件上下文容器
    std::vector<FdContext*> m_fdContexts;
};
}

#endif