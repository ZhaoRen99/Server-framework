#ifndef __SYLAR_FIBER_H__
#define __SYLAR_FIBER_H__

#include <memory>
#include <functional>
#include <ucontext.h>

#include "thread.h"

namespace sylar {

class Fiber : public std::enable_shared_from_this<Fiber> {
public:
    typedef std::shared_ptr<Fiber> ptr;

    enum State
    {
        // ��ʼ��
        INIT,
        // ����
        HOLD,
        // ִ��
        EXEC,
        // ����
        TERM,
        // ׼��
        READY,
        // �쳣
        EXCEPT
    };

private:
    // ��Э��
    Fiber();

public:
    // ��Э��
    Fiber(std::function<void()> cb, size_t stacksize = 0, bool use_caller = false);
    ~Fiber();

    // ����Э�̺����� ������״̬
    void reset(std::function<void()> cb);
    // �л�����ǰЭ��
    void swapIn();
    // �л�����ִ̨��
    void swapOut();
    void call();
    void back();

    void setState(State s);

    const uint64_t getId() const { return m_id; }
    const State getState() const { return m_state; }

public:
    // ���õ�ǰЭ��
    static void SetThis(Fiber* f);
    // ���ص�ǰЭ��
    static Fiber::ptr GetThis();
    // Э���л�����̨�� ��������ΪReady״̬
    static void YieldToReady();
    // Э���л�����̨�� ��������ΪHold״̬
    static void YieldToHold();
    // ��Э����
    static uint64_t TotalFibers();

    static void MainFunc();
    static void CallerMainFunc();

    static uint64_t GetFiberId();

    static void Yield();
private:
    uint64_t m_id = 0;
    uint32_t m_stacksize = 0;
    State m_state = INIT;

    ucontext_t m_ctx;
    // ջ�ڴ�ռ�
    void* m_stack = nullptr;
    // Э��ִ�з���
    std::function<void()> m_cb;
};
}

#endif