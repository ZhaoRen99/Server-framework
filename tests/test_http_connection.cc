/*
 * @Author: wangzhaoren <wangzhaoren99@163.com>
 * @Date: 2025-09-05 14:35:10
 * @LastEditors: wangzhaoren <wangzhaoren99@163.com>
 * @LastEditTime: 2025-09-06 15:57:20
 * @FilePath: /Server-framework/tests/test_http_connection.cc
 * @Description: 
 * 
 * Copyright (c) 2025 by wangzhaoren, All Rights Reserved. 
 */
#include "sylar/http/http_connection.h"
#include "sylar/io/iomanager.h"
#include "sylar/log/log.h"

static SYLAR__ROOT__LOG(g_logger);

void test_pool() {
  // sylar::http::HttpConnectionPool::ptr pool(new
  // sylar::http::HttpConnectionPool(
  //     "www.sylar.top", "", 80, true, 10, 1000 * 30, 5));
  auto pool = sylar::http::HttpConnectionPool::Create("http://www.sylar.top:80",
                                                      "", 10, 1000 * 30, 5);
  sylar::IOManager::GetThis()->addTimer(
      1000,
      [pool]() {
        auto r = pool->doGet("/", 300);
        SYLAR_LOG_INFO(g_logger) << r->toString();
      },
      true);
}

void run() {
  sylar::Address::ptr addr =
      sylar::Address::LookupAnyIPAddress("www.sylar.top:80");
  if (!addr) {
    SYLAR_LOG_INFO(g_logger) << "get addr error";
    return;
  }

  sylar::Socket::ptr sock = sylar::Socket::CreateTCP(addr);
  bool rt = sock->connect(addr);
  if (!rt) {
    SYLAR_LOG_INFO(g_logger) << "connect " << addr << "failed";
    return;
  }

  sylar::http::HttpConnection::ptr conn(new sylar::http::HttpConnection(sock));
  sylar::http::HttpRequest::ptr req(new sylar::http::HttpRequest(0x11, false));
  req->setPath("/blog/");
  req->setHeader("host", "www.sylar.top");
  SYLAR_LOG_INFO(g_logger) << "req: " << std::endl << *req;

  conn->sendRequest(req);
  auto rsp = conn->recvResponse();

  if (!rsp) {
    SYLAR_LOG_INFO(g_logger) << "recv response error";
    return;
  }
  SYLAR_LOG_INFO(g_logger) << "rsp:" << std::endl << *rsp;

  SYLAR_LOG_INFO(g_logger) << "===================";

  auto r =
      sylar::http::HttpConnection::DoGet("http://www.sylar.top/blog/", 300);
  SYLAR_LOG_INFO(g_logger) << "result = " << r->result
                           << " error = " << r->error << " rsp = "
                           << (r->response ? r->response->toString() : "");

  SYLAR_LOG_INFO(g_logger) << "===================";
  test_pool();
}

int main(int argc, char** argv) {
  sylar::IOManager iom(2);
  g_logger->setLevel(sylar::LogLevel::INFO);
  iom.schedule(run);
  return 0;
}