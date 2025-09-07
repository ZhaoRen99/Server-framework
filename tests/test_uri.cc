/*
 * @Author: wangzhaoren <wangzhaoren99@163.com>
 * @Date: 2025-09-05 14:35:10
 * @LastEditors: wangzhaoren <wangzhaoren99@163.com>
 * @LastEditTime: 2025-09-06 16:02:43
 * @FilePath: /Server-framework/tests/test_uri.cc
 * @Description:
 *
 * Copyright (c) 2025 by wangzhaoren, All Rights Reserved.
 */
#include <iostream>

#include "sylar/http/uri.h"

int main(int argc, char** argv) {
  sylar::Uri::ptr uri = sylar::Uri::Create("http://www.sylar.top:80/blog");
  std::cout << uri->toString() << std::endl;
  std::cout << uri->getFragment() << std::endl;
  auto addr = uri->createAddress();
  std::cout << *addr << std::endl;
  return 0;
}