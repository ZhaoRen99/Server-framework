/*
 * @Author: wangzhaoren <wangzhaoren99@163.com>
 * @Date: 2025-09-05 14:35:10
 * @LastEditors: wangzhaoren <wangzhaoren99@163.com>
 * @LastEditTime: 2025-09-06 15:56:05
 * @FilePath: /Server-framework/tests/test_env.cc
 * @Description:
 *
 * Copyright (c) 2025 by wangzhaoren, All Rights Reserved.
 */
#include <unistd.h>

#include <fstream>
#include <iostream>

#include "sylar/env/env.h"

struct A {
  A() {
    std::ifstream ifs("/proc/" + std::to_string(getpid()) + "/cmdline",
                      std::ios::binary);
    std::string content;
    content.resize(4096);

    ifs.read(&content[0], content.size());
    content.resize(ifs.gcount());

    for (size_t i = 0; i < content.size(); ++i) {
      std::cout << i << " - " << content[i] << " - " << (int)content[i]
                << std::endl;
    }
    std::cout << "content: " << content << std::endl;
  }
};

static A a;

int main(int argc, char** argv) {
  sylar::EnvMgr::GetInstance()->addHelp("s", "start with the terminal");
  sylar::EnvMgr::GetInstance()->addHelp("d", "run as daemon");
  sylar::EnvMgr::GetInstance()->addHelp("p", "print help");
  if (!sylar::EnvMgr::GetInstance()->init(argc, argv)) {
    sylar::EnvMgr::GetInstance()->printHelp();
    return 0;
  }

  std::cout << "exe=" << sylar::EnvMgr::GetInstance()->getExe() << std::endl;
  std::cout << "cwd=" << sylar::EnvMgr::GetInstance()->getCwd() << std::endl;
  std::cout << "path=" << sylar::EnvMgr::GetInstance()->getEnv("PATH", "xxx")
            << std::endl;
  std::cout << "set env " << sylar::EnvMgr::GetInstance()->setEnv("TEST", "yyy")
            << std::endl;
  std::cout << "test=" << sylar::EnvMgr::GetInstance()->getEnv("TEST", "x")
            << std::endl;

  if (sylar::EnvMgr::GetInstance()->has("p")) {
    sylar::EnvMgr::GetInstance()->printHelp();
  }
  return 0;
}