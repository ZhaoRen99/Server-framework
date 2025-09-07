/*
 * @Author: wangzhaoren <wangzhaoren99@163.com>
 * @Date: 2025-09-05 14:35:10
 * @LastEditors: wangzhaoren <wangzhaoren99@163.com>
 * @LastEditTime: 2025-09-06 15:53:57
 * @FilePath: /Server-framework/tests/test_address.cc
 * @Description:
 *
 * Copyright (c) 2025 by wangzhaoren, All Rights Reserved.
 */

#include "sylar/common/sylar.h"
#include "sylar/net/address.h"

sylar::Logger::ptr g_logger = SYLAR_LOG_ROOT();

void test() {
  SYLAR_LOG_INFO(g_logger) << "begin";

  std::vector<sylar::Address::ptr> addrs;
  bool v = sylar::Address::Lookup(addrs, "www.baidu.com", AF_INET, SOCK_STREAM);
  if (!v) {
    SYLAR_LOG_ERROR(g_logger) << "lookup fail";
  }

  for (size_t i = 0; i < addrs.size(); ++i) {
    SYLAR_LOG_INFO(g_logger) << i << " - " << addrs[i]->toString();
  }
}

void test_iface() {
  std::multimap<std::string, std::pair<sylar::Address::ptr, uint32_t> > results;

  bool v = sylar::Address::GetInterfaceAddresses(results);
  if (!v) {
    SYLAR_LOG_ERROR(g_logger) << "GetInterfaceAddresses fail";
  }
  for (auto& i : results) {
    SYLAR_LOG_INFO(g_logger) << i.first << " - " << i.second.first->toString()
                             << " - " << i.second.second;
  }
}

void test_ip() {
  auto addr = sylar::IPAddress::Create("www.baidu.com");
  if (addr) {
    SYLAR_LOG_INFO(g_logger) << addr->toString();
  }
}

void test_ipv4() {
  auto addr = sylar::IPv4Address::Create("112.80.248.75", 80);
  auto saddr = addr->subnetMask(24);
  auto baddr = addr->broadcastAddress(24);
  auto naddr = addr->networkAddress(24);
  if (addr) {
    SYLAR_LOG_INFO(g_logger) << addr->toString();
  }
  if (saddr) {
    SYLAR_LOG_INFO(g_logger) << saddr->toString();
  }
  if (baddr) {
    SYLAR_LOG_INFO(g_logger) << baddr->toString();
  }
  if (naddr) {
    SYLAR_LOG_INFO(g_logger) << naddr->toString();
  }
}

void test_ipv6() {
  auto addr = sylar::IPv6Address::Create("fe80::215:5dff:fe20:e26a", 80);
  if (addr) {
    SYLAR_LOG_INFO(g_logger) << addr->toString();
  }
  auto saddr = addr->subnetMask(64);
  auto baddr = addr->broadcastAddress(64);
  auto naddr = addr->networkAddress(64);
  if (addr) {
    SYLAR_LOG_INFO(g_logger) << addr->toString();
  }
  if (saddr) {
    SYLAR_LOG_INFO(g_logger) << saddr->toString();
  }
  if (baddr) {
    SYLAR_LOG_INFO(g_logger) << baddr->toString();
  }
  if (naddr) {
    SYLAR_LOG_INFO(g_logger) << naddr->toString();
  }
}

int main(int argc, char** argv) {
  // sylar::IOManager iom;
  // iom.schedule(test);
  // iom.schedule(test_iface);
  // iom.schedule(test_ipv4);

  // test();
  // test_iface();
  // test_ip();
  // test_ipv4();
  test_ipv6();
  return 0;
}