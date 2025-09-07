/*
 * @Author: wangzhaoren <wangzhaoren@airia.cn>
 * @Date: 2025-09-05 15:25:13
 * @LastEditors: wangzhaoren <wangzhaoren@airia.cn>
 * @LastEditTime: 2025-09-05 16:12:05
 * @FilePath: /Server-framework/src/sylar/io/thread.cc
 * @Description
 *
 * Copyright (c) 2025 by wangzhaoren, All Rights Reserved.
 */
#include "sylar/io/thread.h"

#include "sylar/log/log.h"

namespace sylar {

// thread_local只在当前线程作用域下有效
// 指向当前线程
static thread_local Thread* t_thread = nullptr;
// 指向线程名称
static thread_local std::string t_thread_name = "UNKNOW";

static SYLAR__SYSTEM__LOG(g_logger);

Thread* Thread::GetThis() { return t_thread; }

const std::string& Thread::GetName() { return t_thread_name; }

void Thread::SetName(const std::string& name) {
  if (t_thread) {
    t_thread->m_name = name;
  }
  t_thread_name = name;
}

Thread::Thread(std::function<void()> cb, const std::string& name)
    : m_cb(cb), m_name(name) {
  if (m_name.empty()) {
    m_name = "UNKNOW";
  }
  int rt = pthread_create(&m_thread, nullptr, &Thread::run, this);
  if (rt) {
    SYLAR_LOG_ERROR(g_logger)
        << "pthread_creat thread fail, rt = " << rt << "name = " << name;
    throw std::logic_error("pthread_creat error");
  }
  // 在出构造函数之前，确保线程先跑起来, 保证能够初始化id
  m_semaphore.wait();
}

Thread::~Thread() {
  if (m_thread) {
    pthread_detach(m_thread);
  }
}

void Thread::join() {
  if (m_thread) {
    int rt = pthread_join(m_thread, nullptr);
    if (rt) {
      SYLAR_LOG_ERROR(g_logger)
          << "pthread_join thread fail, rt = " << rt << "name = " << m_name;
      throw std::logic_error("pthread_join error");
    }
    m_thread = 0;
  }
}

void* Thread::run(void* arg) {
  Thread* thread = (Thread*)arg;
  t_thread = thread;
  t_thread_name = thread->m_name;
  thread->m_id = sylar::GetThreadId();
  // 设置线程名称
  pthread_setname_np(pthread_self(), thread->m_name.substr(0, 15).c_str());

  // pthread_creat时返回引用 防止函数有智能指针,
  std::function<void()> cb;
  cb.swap(thread->m_cb);

  // 在出构造函数之前，确保线程先跑起来，保证能够初始化id
  thread->m_semaphore.notify();

  cb();
  return 0;
}

}  // namespace sylar