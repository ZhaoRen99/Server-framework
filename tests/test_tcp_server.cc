/*
 * @Author: wangzhaoren <wangzhaoren99@163.com>
 * @Date: 2025-09-05 14:35:10
 * @LastEditors: wangzhaoren <wangzhaoren99@163.com>
 * @LastEditTime: 2025-09-06 15:58:32
 * @FilePath: /Server-framework/tests/test_tcp_server.cc
 * @Description:
 *
 * Copyright (c) 2025 by wangzhaoren, All Rights Reserved.
 */
#include "sylar/common/sylar.h"

static SYLAR__ROOT__LOG(g_logger);

void run() {
  auto addr = sylar::Address::LookupAny("0.0.0.0:8033");
  // auto addr2 = sylar::UinxAddress::ptr(new
  // sylar::UinxAddress("/tmp/unix_addr"));
  std::vector<sylar::Address::ptr> addrs;
  addrs.push_back(addr);
  // addrs.push_back(addr2);

  sylar::TcpServer::ptr tcp_server(new sylar::TcpServer);
  std::vector<sylar::Address::ptr> fails;

  while (!tcp_server->bind(addrs, fails)) {
    sleep(2);
  }
  tcp_server->start();
}

int main(int argc, char** argv) {
  sylar::IOManager iom(2);
  g_logger->setLevel(sylar::LogLevel::INFO);
  iom.schedule(run);
  return 0;
}