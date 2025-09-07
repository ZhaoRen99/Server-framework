/*
 * @Author: wangzhaoren <wangzhaoren99@163.com>
 * @Date: 2025-09-05 15:19:43
 * @LastEditors: wangzhaoren <wangzhaoren99@163.com>
 * @LastEditTime: 2025-09-06 16:05:06
 * @FilePath: /Server-framework/source/include/sylar/http/servlet.h
 * @Description: 
 * 
 * Copyright (c) 2025 by wangzhaoren, All Rights Reserved. 
 */
#ifndef __SYLAR_SERVLET_H__
#define __SYLAR_SERVLET_H__

#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "http.h"
#include "http_session.h"
#include "sylar/io/mutex.h"

namespace sylar {
namespace http {

class Servlet {
 public:
  typedef std::shared_ptr<Servlet> ptr;
  Servlet(const std::string& name) : m_name(name) {}
  virtual ~Servlet() {}
  virtual int32_t handle(sylar::http::HttpRequest::ptr request,
                         sylar::http::HttpResponse::ptr response,
                         sylar::http::HttpSession::ptr session) = 0;

  const std::string& getName() const { return m_name; }

 protected:
  std::string m_name;
};

class FunctionServlet : public Servlet {
 public:
  typedef std::shared_ptr<FunctionServlet> ptr;
  typedef std::function<int32_t(sylar::http::HttpRequest::ptr request,
                                sylar::http::HttpResponse::ptr response,
                                sylar::http::HttpSession::ptr session)>
      callback;

  FunctionServlet(callback cb);
  virtual int32_t handle(sylar::http::HttpRequest::ptr request,
                         sylar::http::HttpResponse::ptr response,
                         sylar::http::HttpSession::ptr session) override;

 private:
  callback m_cb;
};

class ServletDispatch : public Servlet {
 public:
  typedef std::shared_ptr<ServletDispatch> ptr;
  typedef sylar::RWMutex RWMutexType;

  ServletDispatch();
  virtual int32_t handle(sylar::http::HttpRequest::ptr request,
                         sylar::http::HttpResponse::ptr response,
                         sylar::http::HttpSession::ptr session) override;

  void addServlet(const std::string& uri, Servlet::ptr slt);
  void addServlet(const std::string& uri, FunctionServlet::callback cb);
  void addGlobServlet(const std::string& uri, Servlet::ptr slt);
  void addGlobServlet(const std::string& uri, FunctionServlet::callback cb);

  void delServlet(const std::string& uri);
  void delGlobServlet(const std::string& uri);

  Servlet::ptr getDefault() const { return m_default; }
  void setDefault(Servlet::ptr v) { m_default = v; }

  Servlet::ptr getServlet(const std::string& uri);
  Servlet::ptr getGlobServlet(const std::string& uri);

  Servlet::ptr getMatchedServlet(const std::string& uri);

 private:
  RWMutexType m_mutex;
  // uri(/sylar/xxx) -> servlet
  std::unordered_map<std::string, Servlet::ptr> m_datas;
  // uri(/sylar/*) -> servlet
  std::vector<std::pair<std::string, Servlet::ptr> > m_globs;
  // 默认servlet，所有路径无匹配使用
  Servlet::ptr m_default;
};

class NotFoundServlet : public Servlet {
 public:
  typedef std::shared_ptr<NotFoundServlet> ptr;

  NotFoundServlet(const std::string& name);
  virtual int32_t handle(sylar::http::HttpRequest::ptr request,
                         sylar::http::HttpResponse::ptr response,
                         sylar::http::HttpSession::ptr session) override;

 private:
  std::string m_name;
  std::string m_content;
};

}  // namespace http
}  // namespace sylar

#endif