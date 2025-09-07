/*
 * @Author: wangzhaoren <wangzhaoren99@163.com>
 * @Date: 2025-09-05 14:35:10
 * @LastEditors: wangzhaoren <wangzhaoren99@163.com>
 * @LastEditTime: 2025-09-06 16:00:46
 * @FilePath: /Server-framework/tests/test_util.cc
 * @Description:
 *
 * Copyright (c) 2025 by wangzhaoren, All Rights Reserved.
 */
#include <assert.h>

#include "sylar/common/sylar.h"

sylar::Logger::ptr g_logger = SYLAR_LOG_ROOT();

void test_assert() {
  SYLAR_LOG_INFO(g_logger) << "\n" << sylar::BacktraceToString(10);
  SYLAR_ASSERT2(0 == 1, "Adasd");
}

int main(int argc, char** argv) {
  test_assert();

  return 0;
}