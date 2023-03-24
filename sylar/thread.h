#ifndef __SYLAR_THREAD_H__
#define __SYLAR_THREAD_H__

#include "mutex.h"

namespace sylar {

class Thread {
public:
    typedef std::shared_ptr<Thread> ptr;
    Thread(std::function<void()> cb, const std::string &name);
    ~Thread();

    pid_t getId() const { return m_id; }
    const std::string &getName() const { return m_name; }

    void join();

    /**
     * @brief ��õ�ǰ�߳�
    */
    static Thread *GetThis();

    /**
     * @brief ��־ϵͳ��ȡ�����߳�����
    */
    static const std::string &GetName();

    /**
     * @brief ���̲߳������Լ������ģ����Ը����߳�����
    */
    static void SetName(const std::string& name);
private:
    // ��ֹ��������
    Thread(const Thread &) = delete;
    Thread(const Thread &&) = delete;
    Thread &operator=(const Thread &) = delete;

    static void* run(void* arg); 
private:
    // �߳�id 
    pid_t m_id = -1;
    // �߳̽ṹ
    pthread_t m_thread = 0;
    // �߳�ִ�к���
    std::function<void()> m_cb;
    // �߳�����
    std::string m_name;
    // �ź���
    Semaphore m_semaphore;
};
}

#endif