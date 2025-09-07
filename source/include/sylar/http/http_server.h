/*
 * @Author: wangzhaoren <wangzhaoren99@163.com>
 * @Date: 2025-09-05 15:19:43
 * @LastEditors: wangzhaoren <wangzhaoren99@163.com>
 * @LastEditTime: 2025-09-06 16:04:21
 * @FilePath: /Server-framework/source/include/sylar/http/http_server.h
 * @Description:
 *
 * Copyright (c) 2025 by wangzhaoren, All Rights Reserved.
 */
#ifndef __SYLAR_HTTP_SERVER_H__
#define __SYLAR_HTTP_SERVER_H__

#include "http_session.h"
#include "servlet.h"
#include "sylar/net/tcp_server.h"

namespace sylar {
namespace http {

class HttpServer : public TcpServer {
 public:
  typedef std::shared_ptr<HttpServer> ptr;
  HttpServer(bool keepalive = false,
             sylar::IOManager* worker = sylar::IOManager::GetThis(),
             sylar::IOManager* accept_worker = sylar::IOManager::GetThis());

  ServletDispatch::ptr getServletDispatch() const { return m_dispatch; }
  void setServletDispatch(ServletDispatch::ptr v) { m_dispatch = v; }

  virtual void setName(const std::string& v) override;

 protected:
  void handleClient(Socket::ptr client) override;

 private:
  bool m_isKeepalive;
  ServletDispatch::ptr m_dispatch;
};

}  // namespace http
}  // namespace sylar

#endif