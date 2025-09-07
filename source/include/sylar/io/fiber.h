/*
 * @Author: wangzhaoren <wangzhaoren99@163.com>
 * @Date: 2025-09-05 15:19:43
 * @LastEditors: wangzhaoren <wangzhaoren99@163.com>
 * @LastEditTime: 2025-09-06 16:05:39
 * @FilePath: /Server-framework/source/include/sylar/io/fiber.h
 * @Description: 
 * 
 * Copyright (c) 2025 by wangzhaoren, All Rights Reserved. 
 */
#ifndef __SYLAR_FIBER_H__
#define __SYLAR_FIBER_H__

#include <ucontext.h>

#include <functional>
#include <memory>

#include "thread.h"

namespace sylar {

class Fiber : public std::enable_shared_from_this<Fiber> {
 public:
  typedef std::shared_ptr<Fiber> ptr;

  enum State {
    // 初始化
    INIT,
    // 保持
    HOLD,
    // 执行
    EXEC,
    // 结束
    TERM,
    // 准备
    READY,
    // 异常
    EXCEPT
  };

 private:
  // 主协程
  Fiber();

 public:
  // 子协程
  Fiber(std::function<void()> cb, size_t stacksize = 0,
        bool use_caller = false);
  ~Fiber();

  // 重置协程函数， 并重置状态
  void reset(std::function<void()> cb);
  // 切换到当前协程
  void swapIn();
  // 切换到后台执行
  void swapOut();
  void call();
  void back();

  void setState(State s);

  uint64_t getId() const { return m_id; }
  State getState() const { return m_state; }

 public:
  // 设置当前协程
  static void SetThis(Fiber* f);
  // 返回当前协程
  static Fiber::ptr GetThis();
  // 协程切换到后台， 并且设置为Ready状态
  static void YieldToReady();
  // 协程切换到后台， 并且设置为Hold状态
  static void YieldToHold();
  // 总协程数
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
  // 栈内存空间
  void* m_stack = nullptr;
  // 协程执行方法
  std::function<void()> m_cb;
};
}  // namespace sylar

#endif