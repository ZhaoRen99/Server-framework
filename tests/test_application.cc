/*
 * @Author: wangzhaoren <wangzhaoren99@163.com>
 * @Date: 2025-09-05 14:35:10
 * @LastEditors: wangzhaoren <wangzhaoren99@163.com>
 * @LastEditTime: 2025-09-06 16:03:30
 * @FilePath: /Server-framework/tests/test_application.cc
 * @Description:
 *
 * Copyright (c) 2025 by wangzhaoren, All Rights Reserved.
 */

#include "sylar/app/application.h"

int main(int argc, char** argv) {
  sylar::Application app;
  if (app.init(argc, argv)) {
    return app.run();
  }

  return 0;
}