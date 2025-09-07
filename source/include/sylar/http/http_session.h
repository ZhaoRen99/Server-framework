/*
 * @Author: wangzhaoren <wangzhaoren99@163.com>
 * @Date: 2025-09-05 15:19:43
 * @LastEditors: wangzhaoren <wangzhaoren99@163.com>
 * @LastEditTime: 2025-09-06 16:04:26
 * @FilePath: /Server-framework/source/include/sylar/http/http_session.h
 * @Description: 
 * 
 * Copyright (c) 2025 by wangzhaoren, All Rights Reserved. 
 */
#ifndef __SYLAR_HTTP_SESSION_H__
#define __SYLAR_HTTP_SESSION_H__

#include "http.h"
#include "sylar/streams/socket_stream.h"

namespace sylar {
namespace http {

class HttpSession : public SocketStream {
 public:
  typedef std::shared_ptr<HttpSession> ptr;
  HttpSession(Socket::ptr sock, bool owern = true);
  HttpRequest::ptr recvRequest();
  int sendResponse(HttpResponse::ptr rsp);
};

}  // namespace http
}  // namespace sylar

#endif