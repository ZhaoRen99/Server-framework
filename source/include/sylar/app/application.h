/*
 * @Author: wangzhaoren <wangzhaoren99@163.com>
 * @Date: 2025-09-05 15:19:43
 * @LastEditors: wangzhaoren <wangzhaoren99@163.com>
 * @LastEditTime: 2025-09-06 15:38:06
 * @FilePath: /Server-framework/source/include/sylar/app/application.h
 * @Description:
 *
 * Copyright (c) 2025 by wangzhaoren, All Rights Reserved.
 */
#ifndef __SYLAR_APPLICATION_H__
#define __SYLAR_APPLICATION_H__

#include "sylar/http/http_server.h"

namespace sylar {
class Application {
 public:
  Application();

  static Application* GetInstance() { return s_instance; }
  bool init(int argc, char** argv);
  bool run();

 private:
  int main(int argc, char** argv);
  int run_fiber();

 private:
  int m_argc = 0;
  char** m_argv = nullptr;

  std::vector<sylar::http::HttpServer::ptr> m_httpservers;
  static Application* s_instance;
};
}  // namespace sylar

#endif