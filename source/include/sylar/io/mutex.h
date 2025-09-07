/*
 * @Author: wangzhaoren <wangzhaoren99@163.com>
 * @Date: 2025-09-05 15:19:43
 * @LastEditors: wangzhaoren <wangzhaoren99@163.com>
 * @LastEditTime: 2025-09-06 16:05:48
 * @FilePath: /Server-framework/source/include/sylar/io/mutex.h
 * @Description: 
 * 
 * Copyright (c) 2025 by wangzhaoren, All Rights Reserved. 
 */
#ifndef __SYLAR_MUTEX_H__
#define __SYLAR_MUTEX_H__

#include <pthread.h>
#include <semaphore.h>
#include <stdint.h>

#include <atomic>
#include <functional>
#include <list>
#include <memory>
#include <string>
#include <thread>

#include "sylar/common/noncopyable.h"

namespace sylar {

class Semaphore : public Noncopyable {
 public:
  Semaphore(uint32_t count = 0);
  ~Semaphore();

  void wait();
  void notify();

 private:
  sem_t m_semaphore;
};

template <class T>
struct ScopedLockImpl {
 public:
  ScopedLockImpl(T& mutex) : m_mutex(mutex) {
    m_mutex.lock();
    m_locked = true;
  }

  ~ScopedLockImpl() { unlock(); }

  void lock() {
    if (!m_locked) {
      m_mutex.lock();
      m_locked = true;
    }
  }

  void unlock() {
    if (m_locked) {
      m_mutex.unlock();
      m_locked = false;
    }
  }

 private:
  T& m_mutex;
  bool m_locked;
};

template <class T>
struct ReadScopedLockImpl {
 public:
  ReadScopedLockImpl(T& mutex) : m_mutex(mutex) {
    m_mutex.rdlock();
    m_locked = true;
  }

  ~ReadScopedLockImpl() { unlock(); }

  void lock() {
    if (!m_locked) {
      m_mutex.rdlock();
      m_locked = true;
    }
  }

  void unlock() {
    if (m_locked) {
      m_mutex.unlock();
      m_locked = false;
    }
  }

 private:
  T& m_mutex;
  bool m_locked;
};

template <class T>
struct WriteScopedLockImpl {
 public:
  WriteScopedLockImpl(T& mutex) : m_mutex(mutex) {
    m_mutex.wrlock();
    m_locked = true;
  }

  ~WriteScopedLockImpl() { unlock(); }

  void lock() {
    if (!m_locked) {
      m_mutex.wrlock();
      m_locked = true;
    }
  }

  void unlock() {
    if (m_locked) {
      m_mutex.unlock();
      m_locked = false;
    }
  }

 private:
  T& m_mutex;
  bool m_locked;
};

class NullMutex : public Noncopyable {
 public:
  typedef ScopedLockImpl<NullMutex> Lock;

  NullMutex() {}
  ~NullMutex() {}
  void lock() {}
  void unlock() {}
};

class Mutex : public Noncopyable {
 public:
  typedef ScopedLockImpl<Mutex> Lock;

  Mutex() { pthread_mutex_init(&m_mutex, nullptr); }

  ~Mutex() { pthread_mutex_destroy(&m_mutex); }

  void lock() { pthread_mutex_lock(&m_mutex); }

  void unlock() { pthread_mutex_unlock(&m_mutex); }

 private:
  pthread_mutex_t m_mutex;
};

class NullRWMutex : public Noncopyable {
 public:
  typedef ReadScopedLockImpl<NullRWMutex> ReadLock;
  typedef WriteScopedLockImpl<NullRWMutex> WriteLock;

  NullRWMutex() {}
  ~NullRWMutex() {}
  void rdlock() {}
  void wrlock() {}
  void unlock() {}
};

// 读写锁
class RWMutex : public Noncopyable {
 public:
  typedef ReadScopedLockImpl<RWMutex> ReadLock;
  typedef WriteScopedLockImpl<RWMutex> WriteLock;

  RWMutex() { pthread_rwlock_init(&m_lock, nullptr); }

  ~RWMutex() { pthread_rwlock_destroy(&m_lock); }

  void rdlock() { pthread_rwlock_rdlock(&m_lock); }

  void wrlock() { pthread_rwlock_wrlock(&m_lock); }

  void unlock() { pthread_rwlock_unlock(&m_lock); }

 private:
  pthread_rwlock_t m_lock;
};

// 自旋锁
class Spinlock : public Noncopyable {
 public:
  typedef ScopedLockImpl<Spinlock> Lock;

  Spinlock() { pthread_spin_init(&m_mutex, 0); }

  ~Spinlock() { pthread_spin_destroy(&m_mutex); }

  void lock() { pthread_spin_lock(&m_mutex); }

  void unlock() { pthread_spin_unlock(&m_mutex); }

 private:
  pthread_spinlock_t m_mutex;
};

// 原子锁
class CASLock : public Noncopyable {
 public:
  typedef ScopedLockImpl<CASLock> Lock;

  CASLock() { m_mutex.clear(); }

  ~CASLock() {}

  void lock() {
    while (std::atomic_flag_test_and_set_explicit(&m_mutex,
                                                  std::memory_order_acquire));
  }

  void unlock() {
    std::atomic_flag_clear_explicit(&m_mutex, std::memory_order_release);
  }

 private:
  // volatle 保证对地址的稳定访问， 编译器会优化直接用上次读取的数据赋值
  volatile std::atomic_flag m_mutex;
};
}  // namespace sylar

#endif