/*
 * @Author: wangzhaoren <wangzhaoren99@163.com>
 * @Date: 2025-09-05 15:19:43
 * @LastEditors: wangzhaoren <wangzhaoren99@163.com>
 * @LastEditTime: 2025-09-06 16:05:54
 * @FilePath: /Server-framework/source/include/sylar/io/thread.h
 * @Description: 
 * 
 * Copyright (c) 2025 by wangzhaoren, All Rights Reserved. 
 */
#ifndef __SYLAR_THREAD_H__
#define __SYLAR_THREAD_H__

#include "sylar/common/noncopyable.h"
#include "mutex.h"

namespace sylar {

class Thread : public Noncopyable {
 public:
  typedef std::shared_ptr<Thread> ptr;
  Thread(std::function<void()> cb, const std::string& name);
  ~Thread();

  pid_t getId() const { return m_id; }
  const std::string& getName() const { return m_name; }

  void join();

  /**
   * @brief 获得当前线程
   */
  static Thread* GetThis();

  /**
   * @brief 日志系统获取当先线程名称
   */
  static const std::string& GetName();

  /**
   * @brief 主线程并不是自己创建的，可以给主线程命名
   */
  static void SetName(const std::string& name);

 private:
  static void* run(void* arg);
  // 禁止拷贝构造
  // Thread(const Thread &) = delete;
  // Thread(const Thread &&) = delete;
  // Thread &operator=(const Thread &) = delete;

 private:
  // 线程id
  pid_t m_id = -1;
  // 线程结构
  pthread_t m_thread = 0;
  // 线程执行函数
  std::function<void()> m_cb;
  // 线程名称
  std::string m_name;
  // 信号量
  Semaphore m_semaphore;
};
}  // namespace sylar

#endif