/*
 * @Author: wangzhaoren <wangzhaoren99@163.com>
 * @Date: 2025-09-05 15:19:43
 * @LastEditors: wangzhaoren <wangzhaoren99@163.com>
 * @LastEditTime: 2025-09-06 16:42:40
 * @FilePath: /Server-framework/source/include/sylar/streams/socket_stream.h
 * @Description: 
 * 
 * Copyright (c) 2025 by wangzhaoren, All Rights Reserved. 
 */
#ifndef __SYLAR_SOCKET_STREAM_H__
#define __SYLAR_SOCKET_STREAM_H__

#include "sylar/net/socket.h"
#include "sylar/net/stream.h"

namespace sylar {

class SocketStream : public Stream {
 public:
  typedef std::shared_ptr<SocketStream> ptr;
  SocketStream(Socket::ptr sock, bool owner = true);
  ~SocketStream();

  virtual int read(void* buffer, size_t length) override;
  virtual int read(ByteArray::ptr ba, size_t length) override;

  virtual int write(const void* buffer, size_t length) override;
  virtual int write(ByteArray::ptr ba, size_t length) override;

  virtual void close() override;

  Socket::ptr getSocket() const { return m_socket; }
  bool isConnected() const;

 protected:
  Socket::ptr m_socket;
  bool m_owner;
};

}  // namespace sylar

#endif