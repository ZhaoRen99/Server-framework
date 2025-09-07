/*
 * @Author: wangzhaoren <wangzhaoren99@163.com>
 * @Date: 2025-09-05 15:19:43
 * @LastEditors: wangzhaoren <wangzhaoren99@163.com>
 * @LastEditTime: 2025-09-06 16:04:07
 * @FilePath: /Server-framework/source/include/sylar/daemon/daemon.h
 * @Description: 
 * 
 * Copyright (c) 2025 by wangzhaoren, All Rights Reserved. 
 */
#ifndef __SYLAR_DAEMON_H__
#define __SYLAR_DAEMON_H__

#include <unistd.h>

#include <functional>

#include "sylar/common/singleton.h"

namespace sylar {

struct ProcessInfo {
  // 父进程id
  pid_t parent_id;
  // 主进程id
  pid_t main_id;
  // 父进程启动时间
  uint64_t parent_start_time = 0;
  // 主进程启动时间
  uint64_t main_start_time = 0;
  // 主进程重启的次数
  uint32_t restart_count = 0;

  std::string toString();
};

typedef sylar::Singleton<ProcessInfo> ProcessInfoMgr;

/**
 * @brief 启动程序，选择用守护进程的方式
 *
 * @param argc 参数个数
 * @param argv 参数值数组
 * @param main_cb 启动函数
 * @param is_daemon 是否守护进程
 * @return int 返回函数执行结果
 */
int start_daemon(int argc, char** argv,
                 std::function<int(int argc, char** argv)> main_cb,
                 bool is_daemon);
}  // namespace sylar

#endif