#ifndef __SYLAR_SCHEDULER_H__
#define __SYLAR_SCHEDULER_H__

#include <memory>
#include <vector>

#include "fiber.h"
#include "mutex.h"
#include "thread.h"
#include "log.h"
#include "hook.h"

namespace sylar {

/**
 * @brief Э�̵�����
 * @details ��װ N-M ��Э�̵�����
 *          �ڲ�һ���̳߳أ�֧��Э�����̳߳����л�
*/
class Scheduler {
public:
    typedef Mutex MutexType;
    typedef std::shared_ptr<Scheduler> ptr;

    /**
     * @brief ���캯��
     * @param[in] threads �߳�����
     * @param[in] use_caller �Ƿ�ʹ�õ�ǰ�����߳�
     *                       ���ĸ��߳�ִ����Э�̵������Ĺ��캯��
     *                       ��Ϊtrue��������߳�Ҳ�����������
     * @param[in] name Э�̵���������
    */
    Scheduler(size_t threads = 1, bool use_caller = true, const std::string &name = "");
    
    /**
     * @brief ��������
    */
    virtual ~Scheduler();

    /**
     * @brief ����Э�̵���������
    */
    const std::string &getName() const { return m_name; }

    /**
     * @brief ���ص�ǰЭ�̵�����
    */
    static Scheduler* GetThis();

    /**
     * @brief ���ص�ǰЯ�̵������ĵ���Э��
     *        ��Э�� ÿ��Scheduler����Э�̸����̵߳�����
    */
    static Fiber* GetMainFiber();

    /**
     * @brief ����Э�̵�����
    */
    void start();

    /**
     * @brief ֹͣЭ�̵�����
    */
    void stop();

    /**
     * @brief ����Э��
     * @param[in] fc Э�̻���
     * @param[in] thread Э��ִ�е�id�� -1��ʾ����Э��
    */
    template<class FiberOrCb>
    void schedule(FiberOrCb fc, int thread = -1) {
        bool need_tickle = false;
        {
            MutexType::Lock lock(m_mutex);
            need_tickle = scheduleNoLock(fc, thread);
        }

        if (need_tickle) {
            tickle();
        }
    }
    /**
     * @brief ��������Э��
     * @param[in] begin Э������Ŀ�ʼ
     * @param[in] end Э������Ľ���
    */
    template<class InputIterator>
    void schedule(InputIterator begin, InputIterator end) {
        bool need_tickle = false;
        {
            MutexType::Lock lock(m_mutex);
            while (begin != end) {
                need_tickle = scheduleNoLock(&*begin, -1) || need_tickle;
                ++begin;
            }
        }
        if (need_tickle) {
            tickle();
        }
    }

protected:
    /**
     * @brief ֪ͨЭ�̵�������������
    */
    virtual void tickle();

    /**
     * @brief �����Ƿ����ֹͣ
    */
    virtual bool stopping();

    /**
     * @brief Э��������ɵ���ʱִ��idleЭ��
    */
    virtual void idle();

    /**
    * @brief Э�̵��Ⱥ���
    */
    void run();

    /**
     * @brief ���õ�ǰЭ�̵�����
    */
    void setThis();

    /**
     * @brief �Ƿ��п����߳�
     * 
     * @return true 
     * @return false 
     */
    bool hasIdleThread() { return m_idleThreadCount > 0; }

private:
    /**
     * @brief Э�̵�������(����)
    */
    template<class FiberOrCb>
    bool scheduleNoLock(FiberOrCb fc, int thread) {
        bool need_tickle = m_fibers.empty();
        FiberAndThread ft(fc, thread);
        if (ft.fiber || ft.cb) {
            m_fibers.push_back(ft);
        }
        return need_tickle;
    }

private:
    /**
     * @brief Э��/����/�߳���
    */
    struct FiberAndThread {
        // Э��
        Fiber::ptr fiber;
        // Э��ִ�к���
        std::function<void()> cb;
        // �߳�id Э�����ĸ��߳��� 
        int thread;

        /**
         * @brief ���캯��
         *        Э�����ĸ��߳�����
         * @param[in] f Э��
         * @param[in] thr �߳�id
        */
        FiberAndThread(Fiber::ptr f, int thr)
            :fiber(f), thread(thr) {
        }
        
        /**
         * @brief ���캯��
         * @param[in] f Э��ָ��
         *              ����ָ��ʹ�����ü���-1
         * @param[in] thr �߳�id
         * @post *f = nullptr
        */
        FiberAndThread(Fiber::ptr* f, int thr)
            :thread(thr) {
            fiber.swap(*f);
        }

        /**
         * @brief ���캯��
         * @param[in] f Э��ִ�к���
         * @param[in] thr �߳�id
        */
        FiberAndThread(std::function<void()> f, int thr)
            :cb(f), thread(thr) {
        }

        /**
         * @brief ���캯��
         * @param[in] f Э��ִ�к���ָ��
         *              ����ָ��ʹ�����ü���-1
         * @param[in] thr �߳�idSYLAR_LOG_ROOT()
         * @post *f = nullptr
        */
        FiberAndThread(std::function<void()>* f, int thr)
            :thread(thr) {
            cb.swap(*f);
        }

        /**
         * @brief �޲ι��캯��
        */
        FiberAndThread() {
            thread = -1;
        }

        /**
         * @brief ��������
        */
        void reset() {
            fiber = nullptr;
            cb = nullptr;
            thread = -1;
        }

    };

private:
    // Mutex
    MutexType m_mutex;
    // �̳߳�
    std::vector<sylar::Thread::ptr> m_threads;
    // ��ִ�е�Э�̶���
    std::list<FiberAndThread> m_fibers;
    // use_callerΪtrueʱ��Ч������Э��
    Fiber::ptr m_rootFiber;
    // Э�̵���������
    std::string m_name;

protected:
    // Э���µ��߳�id����
    std::vector<int> m_threadIds;
    // �߳�����
    size_t m_threadCount = 0;
    // �����߳�����
    std::atomic<size_t> m_activateThreadCount = {0};
    // �����߳�����
    std::atomic<size_t> m_idleThreadCount = {0};
    // �Ƿ�����ֹͣ
    bool m_stopping = true;
    // �Ƿ��Զ�ֹͣ
    bool m_autoStop = false;
    //  ���߳�Id(use_caller)
    int m_rootThread = 0;
};
}

#endif