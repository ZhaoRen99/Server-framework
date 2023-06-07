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
        // ���¼�
        NONE    = 0x0,
        // ���¼�(EPOLLIN)
        READ    = 0x1,
        // д�¼�(EPOLLOUT)
        WRITE   = 0x4,

    };

private:
    /**
     * @brief socket�¼�������
     * 
     */
    struct FdContext {
        typedef Mutex MutexType;
        /**
         * @brief �¼�������
         * 
         */
        struct EventContext {
            Scheduler* scheduler = nullptr; //�¼�ִ�е�scheduler
            Fiber::ptr fiber;               //�¼�Э��
            std::function<void()> cb;       //�¼�ִ�еĻص�����
        };

        /**
         * @brief ��ȡ��Ӧ�¼�������
         * 
         * @param event 
         * @return EventContext& 
         */
        EventContext& getContext(Event event);

        /**
         * @brief �����¼�������
         * 
         * @param ctx 
         */
        void resetContext(EventContext& ctx);

        /**
         * @brief �����¼�
         * 
         * @param event 
         */
        void triggerEvent(Event event);

        EventContext read;      //���¼�
        EventContext write;     //д�¼�
        int fd;                 //�¼������ľ��
        Event events = NONE;    //�Ѿ�ע����¼�
        MutexType mutex;        
    };

public:
    /**
     * @brief ���캯��
     * 
     * @param threads   �߳����� 
     * @param use_caller    �Ƿ񽫵����̰߳�����ȥ
     * @param name  ����������
     */
    IOManager(size_t threads = 1, bool use_caller = true, const std::string &name = "main");
    
    /**
     * @brief ��������
     * 
     */
    ~IOManager();

    /**
     * @brief ����¼�
     * 
     * @param fd socket���
     * @param event �¼�����
     * @param cb �¼��ص�����
     * @return int �ɹ�����0�� ʧ�ܷ���-1
     */
    int addEvent(int fd, Event event, std::function<void()> cb = nullptr);

    /**
     * @brief ɾ���¼�
     * 
     * @param fd socket���
     * @param event �¼�����
     * @return true 
     * @return false 
     * @attention ���ᴥ���¼�
     */
    bool delEvent(int fd, Event event);

    /**
     * @brief ȡ��ʱ��
     * 
     * @param fd socket���
     * @param event �¼�����
     * @return true 
     * @return false 
     * @attention ����¼������򴥷��¼�
     */
    bool cancelEvent(int fd, Event event);

    /**
     * @brief ȡ�������¼�
     * 
     * @param fd socket���
     * @return true 
     * @return false 
     */
    bool cancelAll(int fd);

    /**
     * @brief ��õ�ǰIOManager
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
     * @brief ����socket��������ĵ�������С
     * 
     * @param size ������С
     */
    void contextResize(size_t size);

    /**
     * @brief �ж��Ƿ����ֹͣ
     * 
     * @param timeout ���Ҫ�����Ķ�ʱ���¼����
     * @return true 
     * @return false 
     */
    bool stopping(uint64_t& timeout);

private:
    //epoll�ļ����
    int m_epfd = 0; 
    // pipe�ļ����������fd[0]��ʾ���ˣ�fd[1] ��ʾд��
    int m_tickleFds[2];
    // �ȴ�ִ�е��¼�����
    std::atomic<size_t> m_pendingEventCount = {0}; 
    RWMutexType m_mutex;
    //socket�¼�����������
    std::vector<FdContext*> m_fdContexts;
};
}

#endif