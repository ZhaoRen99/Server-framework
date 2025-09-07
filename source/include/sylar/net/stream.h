/*
 * @Author: wangzhaoren <wangzhaoren99@163.com>
 * @Date: 2025-09-05 15:19:43
 * @LastEditors: wangzhaoren <wangzhaoren99@163.com>
 * @LastEditTime: 2025-09-06 16:07:09
 * @FilePath: /Server-framework/source/include/sylar/net/stream.h
 * @Description:
 *
 * Copyright (c) 2025 by wangzhaoren, All Rights Reserved.
 */
#ifndef __SYLAR_STREAM_H__
#define __SYLAR_STREAM_H__

#include <memory>

#include "bytearray.h"

namespace sylar {

class Stream {
 public:
  typedef std::shared_ptr<Stream> ptr;
  virtual ~Stream() {}

  virtual int read(void* buffer, size_t length) = 0;
  virtual int read(ByteArray::ptr ba, size_t length) = 0;
  virtual int readFixSize(void* buffer, size_t length);
  virtual int readFixSize(ByteArray::ptr ba, size_t length);

  virtual int write(const void* buffer, size_t length) = 0;
  virtual int write(ByteArray::ptr ba, size_t length) = 0;
  virtual int writeFixSize(const void* buffer, size_t length);
  virtual int writeFixSize(ByteArray::ptr ba, size_t length);

  virtual void close() = 0;
};

}  // namespace sylar

#endif