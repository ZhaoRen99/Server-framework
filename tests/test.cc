/*
 * @Author: wangzhaoren <wangzhaoren99@163.com>
 * @Date: 2025-09-05 14:35:10
 * @LastEditors: wangzhaoren <wangzhaoren99@163.com>
 * @LastEditTime: 2025-09-07 18:21:54
 * @FilePath: /Server-framework/tests/test.cc
 * @Description:
 *
 * Copyright (c) 2025 by wangzhaoren, All Rights Reserved.
 */
#include <iostream>

#include "sylar/common/util.h"
#include "sylar/log/log.h"
#include "version.hpp"

int main(int argc, char** argv) {
  std::cout << "\nhello sylar log" << std::endl;

  // sylar::Logger::ptr logger(new sylar::Logger("zhaoren"));
  // logger->addAppender(sylar::LogAppender::ptr(new sylar::StdoutLogAppender));

  // sylar::FileLogAppender::ptr file_appender(new
  // sylar::FileLogAppender("./log.txt")); sylar::LogFormatter::ptr fmt(new
  // sylar::LogFormatter("%d%T%p%T%m%n")); file_appender->setFormatter(fmt);
  // file_appender->setLevel(sylar::LogLevel::ERROR);

  // logger->addAppender(file_appender);

  // // 流
  // SYLAR_LOG_INFO(logger) << "test macro";
  // SYLAR_LOG_ERROR(logger) << "test error";

  // // format
  // SYLARY_LOG_FMT_ERROR(logger, "test macro fmt error %s", "aa");

  // sylar::Logger::ptr l
  // auto l = sylar::LoggerMgr::GetInstance()->getLogger("");
  // std::cout << std::endl;
  // SYLAR_LOG_INFO (l) << "xxx";
  SYLAR__ROOT__LOG(g_logger);
  SYLAR_LOG_INFO(g_logger) << "hello " << sylar::sylar_build_time();
  
  return 0;
}

// sylar::LogEvent::ptr event(new sylar::LogEvent(__FILE__, __LINE__, 0,
// sylar::GetThreadId(), sylar::GetFiberId(), time(0))); event->getSS() <<
// "hello sylar log"; logger->log(sylar::LogLevel::DEBUG, event);