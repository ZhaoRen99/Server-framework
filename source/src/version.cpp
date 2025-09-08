/*
 * @Author: wangzhaoren <wangzhaoren99@163.com>
 * @Date: 2025-09-05 11:17:23
 * @LastEditors: wangzhaoren <wangzhaoren99@163.com>
 * @LastEditTime: 2025-09-07 18:58:08
 * @FilePath: /Server-framework/cmake/version/version.cpp.in
 * @Description: Version
 *
 * Copyright (c) 2025 by wangzhaoren, All Rights Reserved.
 */

#if !defined(SYLAR_BUILD_TIME)
#define SYLAR_BUILD_TIME "2025-09-08 13:39:02"
#endif

namespace sylar {

__attribute__((visibility("default"))) const char *sylar_build_time() {
  return SYLAR_BUILD_TIME;
}

}  // namespace sylar
