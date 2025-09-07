/*
 * @Author: wangzhaoren <wangzhaoren99@163.com>
 * @Date: 2025-09-05 14:35:10
 * @LastEditors: wangzhaoren <wangzhaoren99@163.com>
 * @LastEditTime: 2025-09-06 15:57:34
 * @FilePath: /Server-framework/tests/test_http_parser.cc
 * @Description:
 *
 * Copyright (c) 2025 by wangzhaoren, All Rights Reserved.
 */
#include "sylar/common/sylar.h"

static SYLAR__ROOT__LOG(g_logger);

const char test_request_data[] =
    "POST / HTTP/1.1\r\n"
    "Host: www.sylar.top\r\n"
    "Content-Length: 10\r\n\r\n"
    "1234567890";

void test_request() {
  sylar::http::HttpRequestParser parser;
  std::string tmp = test_request_data;
  size_t s = parser.execute(&tmp[0], tmp.size());
  SYLAR_LOG_INFO(g_logger) << "execute rt = " << s
                           << " has_error = " << parser.hasError()
                           << " is_finish = " << parser.isFinished()
                           << " total = " << tmp.size()
                           << " content_length = " << parser.getContentLength();

  tmp.resize(tmp.size() - s);
  std::cout << parser.getData()->toString();
  std::cout << tmp << std::endl;
}

const char test_response_data[] =
    "HTTP/1.1 200 OK\r\n"
    "Date: Tue, 04 Jun 2019 15:43:56 GMT\r\n"
    "Server: Apache\r\n"
    "Last-Modified: Tue, 12 Jan 2010 13:48:00 GMT\r\n"
    "ETag: \"51-47cf7e6ee8400\"\r\n"
    "Accept-Ranges: bytes\r\n"
    "Content-Length: 81\r\n"
    "Cache-Control: max-age=86400\r\n"
    "Expires: Wed, 05 Jun 2019 15:43:56 GMT\r\n"
    "Connection: keep-alive\r\n"
    "Set-Cookie: BAIDUID_BFESS=EF8EC2CD8B4B1B7DAD0552BF0CF7EC3E:FG=1; Path=/; "
    "Domain=baidu.com; Expires=Tue, 04 Jun 2024 13:56:34 GMT; "
    "Max-Age=31536000; Secure; SameSite=None\r\n"
    "Content-Type: text/html\r\n\r\n"
    "<html>\r\n"
    "<meta http-equiv=\"refresh\" content=\"0;url=http://www.baidu.com/\">\r\n"
    "</html>\r\n";

void test_response() {
  sylar::http::HttpResponseParser parser;
  std::string tmp = test_response_data;
  size_t s = parser.execute(&tmp[0], tmp.size(), false);
  SYLAR_LOG_INFO(g_logger) << "execute rt = " << s
                           << " has_error = " << parser.hasError()
                           << " is_finished = " << parser.isFinished()
                           << " total = " << tmp.size()
                           << " content_lenght = " << parser.getContentLength();

  tmp.resize(tmp.size() - s);
  std::cout << parser.getData()->toString();
  std::cout << tmp << std::endl;
}

int main(int argc, char** argv) {
  test_request();
  test_response();
  return 0;
}