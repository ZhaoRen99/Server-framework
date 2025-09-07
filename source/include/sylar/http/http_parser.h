/*
 * @Author: wangzhaoren <wangzhaoren99@163.com>
 * @Date: 2025-09-05 15:19:43
 * @LastEditors: wangzhaoren <wangzhaoren99@163.com>
 * @LastEditTime: 2025-09-06 16:04:17
 * @FilePath: /Server-framework/source/include/sylar/http/http_parser.h
 * @Description: 
 * 
 * Copyright (c) 2025 by wangzhaoren, All Rights Reserved. 
 */
#ifndef __SYLAR_HTTP_PARSER_H__
#define __SYLAR_HTTP_PARSER_H__

#include "http.h"
#include "http11_parser.h"
#include "httpclient_parser.h"

namespace sylar {
namespace http {

class HttpRequestParser {
 public:
  typedef std::shared_ptr<HttpRequestParser> ptr;
  HttpRequestParser();

  size_t execute(char* data, size_t len);
  int isFinished();
  int hasError();

  HttpRequest::ptr getData() const { return m_data; }
  void setError(int v) { m_error = v; }

  uint64_t getContentLength();
  const http_parser& getParser() const { return m_parser; }

 public:
  static uint64_t GetHttpRequestBufferSize();
  static uint64_t GetHttpRequestMaxBodySize();

 private:
  http_parser m_parser;
  HttpRequest::ptr m_data;
  // 1000: invalid method
  // 1001: invalid version
  // 1002: invalid filed
  int m_error;
};

class HttpResponseParser {
 public:
  typedef std::shared_ptr<HttpResponseParser> ptr;
  HttpResponseParser();

  size_t execute(char* data, size_t len, bool chunck);
  int isFinished();
  int hasError();

  HttpResponse::ptr getData() const { return m_data; }
  void setError(int v) { m_error = v; }

  uint64_t getContentLength();
  const httpclient_parser& getParser() const { return m_parser; }

 public:
  static uint64_t GetHttpResponseBufferSize();
  static uint64_t GetHttpResponseMaxBodySize();

 private:
  httpclient_parser m_parser;
  HttpResponse::ptr m_data;
  // 1001: invalid version
  // 1002: invalid field
  int m_error;
};

}  // namespace http
}  // namespace sylar

#endif