/*
 * @Author: wangzhaoren <wangzhaoren99@163.com>
 * @Date: 2025-09-05 14:35:10
 * @LastEditors: wangzhaoren <wangzhaoren99@163.com>
 * @LastEditTime: 2025-09-06 16:00:15
 * @FilePath: /Server-framework/tests/test_thread.cc
 * @Description:
 *
 * Copyright (c) 2025 by wangzhaoren, All Rights Reserved.
 */
#include <unistd.h>

#include "sylar/common/sylar.h"

sylar::Logger::ptr g_logger = SYLAR_LOG_ROOT();

int count = 0;
sylar::RWMutex s_mutex;
sylar::Mutex mutex;

void fun1() {
  SYLAR_LOG_INFO(g_logger) << "name: " << sylar::Thread::GetName()
                           << ", this.name: "
                           << sylar::Thread::GetThis()->getName()
                           << ", id: " << sylar::GetThreadId() << ", this.id: "
                           << sylar::Thread::GetThis()->getId();

  for (int i = 0; i < 100000; ++i) {
    sylar::Mutex::Lock lock(mutex);
    ++count;
  }
}

void fun2() {
  while (1) {
    SYLAR_LOG_INFO(g_logger) << "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
  }
}

void fun3() {
  while (1) {
    SYLAR_LOG_INFO(g_logger)
        << "====================================================";
  }
}

int main(int argc, char** argv) {
  SYLAR_LOG_INFO(g_logger) << "thread test begin" << std::endl;
  YAML::Node root =
      YAML::LoadFile("/home/zhaoren/codeworkspace/sylar/bin/conf/log2.yml");
  // sylar::Config::LoadFromYaml(root);

  std::vector<sylar::Thread::ptr> thrs;
  for (int i = 0; i < 2; ++i) {
    sylar::Thread::ptr thr(
        new sylar::Thread(&fun2, "name_" + std::to_string(i * 2)));
    sylar::Thread::ptr thr2(
        new sylar::Thread(&fun3, "name_" + std::to_string(i * 2 + 1)));
    thrs.push_back(thr);
    thrs.push_back(thr2);
  }

  for (size_t i = 0; i < thrs.size(); ++i) {
    thrs[i]->join();
  }

  SYLAR_LOG_INFO(g_logger) << "thread test end" << std::endl;
  SYLAR_LOG_INFO(g_logger) << "count = " << count;

  return 0;
}