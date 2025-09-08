/*
 * @Author: wangzhaoren <wangzhaoren99@163.com>
 * @Date: 2025-09-05 10:56:28
 * @LastEditors: wangzhaoren <wangzhaoren99@163.com>
 * @LastEditTime: 2025-09-06 22:17:28
 * @FilePath: /Server-framework/cmake/version/version.hpp.in
 * @Description: Version
 *
 * Copyright (c) 2025 by wangzhaoren, All Rights Reserved.
 */

#ifndef __VERSION_HPP__
#define __VERSION_HPP__

#define SYLAR_VERSION_MAJOR @SYLAR_VERSION_MAJOR @
#define SYLAR_VERSION_MINOR @SYLAR_VERSION_MINOR @
#define SYLAR_VERSION_PATCH @SYLAR_VERSION_PATCH @
#define SYLAR_VERSION_TWEAK @SYLAR_VERSION_TWEAK @

#define SYLAR_VERSION "0.0.0.1"

#define SYLAR_GIT_HASH "0f887a05f4a4c67f034a308fefb6e6da1f4d0f9f"

namespace sylar {

__attribute__((visibility("default"))) const char *sylar_build_time();

}  // namespace sylar

#endif  // __VERSION_HPP__
