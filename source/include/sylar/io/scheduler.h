/*
 * @Author: wangzhaoren <wangzhaoren99@163.com>
 * @Date: 2025-09-05 15:19:43
 * @LastEditors: wangzhaoren <wangzhaoren99@163.com>
 * @LastEditTime: 2025-09-06 16:05:51
 * @FilePath: /Server-framework/source/include/sylar/io/scheduler.h
 * @Description: 
 * 
 * Copyright (c) 2025 by wangzhaoren, All Rights Reserved. 
 */
#ifndef __SYLAR_SCHEDULER_H__
#define __SYLAR_SCHEDULER_H__

#include <memory>
#include <vector>

#include "fiber.h"
#include "hook.h"
#include "sylar/log/log.h"
#include "mutex.h"
#include "thread.h"

namespace sylar {

/**
 * @brief 协程调度器
 * @details 封装 N-M 的协程调度器
 *          内部一个线程池，支持协程在线程池中切换
 */
class Scheduler {
 public:
  typedef Mutex MutexType;
  typedef std::shared_ptr<Scheduler> ptr;

  /**
   * @brief 构造函数
   * @param[in] threads 线程数量
   * @param[in] use_caller 是否使用当前调用线程
   *                       在哪个线程执行了协程调度器的构造函数
   *                       若为true则会把这个线程也纳入调度其中
   * @param[in] name 协程调度器名称
   */
  Scheduler(size_t threads = 1, bool use_caller = true,
            const std::string& name = "");

  /**
   * @brief 析构函数
   */
  virtual ~Scheduler();

  /**
   * @brief 返回协程调度器名称
   */
  const std::string& getName() const { return m_name; }

  /**
   * @brief 返回当前协程调度器
   */
  static Scheduler* GetThis();

  /**
   * @brief 返回当前携程调度器的调度协程
   *        主协程 每个Scheduler的主协程负责线程的任务
   */
  static Fiber* GetMainFiber();

  /**
   * @brief 启动协程调度器
   */
  void start();

  /**
   * @brief 停止协程调度器
   */
  void stop();

  /**
   * @brief 调度协程
   * @param[in] fc 协程或函数
   * @param[in] thread 协程执行的id， -1表示任意协程
   */
  template <class FiberOrCb>
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
   * @brief 批量调度协程
   * @param[in] begin 协程数组的开始
   * @param[in] end 协程数组的结束
   */
  template <class InputIterator>
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
   * @brief 通知协程调度器有任务了
   */
  virtual void tickle();

  /**
   * @brief 返回是否可以停止
   */
  virtual bool stopping();

  /**
   * @brief 协程无任务可调度时执行idle协程
   */
  virtual void idle();

  /**
   * @brief 协程调度函数
   */
  void run();

  /**
   * @brief 设置当前协程调度器
   */
  void setThis();

  /**
   * @brief 是否有空闲线程
   *
   * @return true
   * @return false
   */
  bool hasIdleThread() { return m_idleThreadCount > 0; }

 private:
  /**
   * @brief 协程调度启动(无锁)
   */
  template <class FiberOrCb>
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
   * @brief 协程/函数/线程组
   */
  struct FiberAndThread {
    // 协程
    Fiber::ptr fiber;
    // 协程执行函数
    std::function<void()> cb;
    // 线程id 协程在哪个线程上
    int thread;

    /**
     * @brief 构造函数
     *        协程在哪个线程上跑
     * @param[in] f 协程
     * @param[in] thr 线程id
     */
    FiberAndThread(Fiber::ptr f, int thr) : fiber(f), thread(thr) {}

    /**
     * @brief 构造函数
     * @param[in] f 协程指针
     *              传空指针使得引用计数-1
     * @param[in] thr 线程id
     * @post *f = nullptr
     */
    FiberAndThread(Fiber::ptr* f, int thr) : thread(thr) { fiber.swap(*f); }

    /**
     * @brief 构造函数
     * @param[in] f 协程执行函数
     * @param[in] thr 线程id
     */
    FiberAndThread(std::function<void()> f, int thr) : cb(f), thread(thr) {}

    /**
     * @brief 构造函数
     * @param[in] f 协程执行函数指针
     *              传空指针使得引用计数-1
     * @param[in] thr 线程idSYLAR_LOG_ROOT()
     * @post *f = nullptr
     */
    FiberAndThread(std::function<void()>* f, int thr) : thread(thr) {
      cb.swap(*f);
    }

    /**
     * @brief 无参构造函数
     */
    FiberAndThread() { thread = -1; }

    /**
     * @brief 重置数据
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
  // 线程池
  std::vector<sylar::Thread::ptr> m_threads;
  // 待执行的协程队列
  std::list<FiberAndThread> m_fibers;
  // use_caller为true时有效，调度协程
  Fiber::ptr m_rootFiber;
  // 协程调度器名称
  std::string m_name;

 protected:
  // 协程下的线程id数组
  std::vector<int> m_threadIds;
  // 线程数量
  size_t m_threadCount = 0;
  // 工作线程数量
  std::atomic<size_t> m_activateThreadCount = {0};
  // 空闲线程数量
  std::atomic<size_t> m_idleThreadCount = {0};
  // 是否正在停止
  bool m_stopping = true;
  // 是否自动停止
  bool m_autoStop = false;
  //  主线程Id(use_caller)
  int m_rootThread = 0;
};
}  // namespace sylar

#endif