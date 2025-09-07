/*
 * @Author: wangzhaoren <wangzhaoren99@163.com>
 * @Date: 2025-09-05 15:19:43
 * @LastEditors: wangzhaoren <wangzhaoren99@163.com>
 * @LastEditTime: 2025-09-06 16:03:52
 * @FilePath: /Server-framework/source/include/sylar/common/singleton.h
 * @Description: 
 * 
 * Copyright (c) 2025 by wangzhaoren, All Rights Reserved. 
 */
#ifndef __SYLAR_SINGLETON_H__
#define __SYLAR_SINGLETON_H__

#include <memory>

namespace sylar {

template <class T, class X = void, int N = 0>
class Singleton {
 public:
  // typedef sylar::Singleton<LoggerManager> LoggerMgr;
  static T* GetInstance() {
    static T v;
    return &v;
  }
};

template <class T, class X = void, int N = 0>
class SingletonPtr {
 public:
  static std::shared_ptr<T> GetInstance() {
    static std::shared_ptr<T> v(new T);
    return v;
  }
};

}  // namespace sylar

#endif