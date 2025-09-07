/*
 * @Author: wangzhaoren <wangzhaoren99@163.com>
 * @Date: 2025-09-05 14:35:10
 * @LastEditors: wangzhaoren <wangzhaoren99@163.com>
 * @LastEditTime: 2025-09-06 16:03:15
 * @FilePath: /Server-framework/tests/test_daemon.cc
 * @Description:
 *
 * Copyright (c) 2025 by wangzhaoren, All Rights Reserved.
 */
#include "sylar/daemon/daemon.h"
#include "sylar/io/iomanager.h"

static SYLAR__ROOT__LOG(g_logger);

static sylar::Timer::ptr timer;

int server_main(int argc, char** argv) {
  sylar::IOManager iom(1);
  SYLAR_LOG_INFO(g_logger) << sylar::ProcessInfoMgr::GetInstance()->toString();
  timer = iom.addTimer(
      1000,
      []() {
        SYLAR_LOG_INFO(g_logger) << "OnTimer";
        static int count = 0;
        if (++count >= 5) {
          timer->cancel();
        }
      },
      true);
  return 0;
}

int main(int argc, char** argv) {
  return sylar::start_daemon(argc, argv, server_main, argc != 1);
}