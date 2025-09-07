/*
 * @Author: wangzhaoren <wangzhaoren99@163.com>
 * @Date: 2025-09-05 15:19:43
 * @LastEditors: wangzhaoren <wangzhaoren99@163.com>
 * @LastEditTime: 2025-09-06 16:05:10
 * @FilePath: /Server-framework/source/include/sylar/http/uri.h
 * @Description: 
 * 
 * Copyright (c) 2025 by wangzhaoren, All Rights Reserved. 
 */
#ifndef __SYLAR_URI_H__
#define __SYLAR_URI_H__

#include <memory>
#include <string>

#include "sylar/net/address.h"

namespace sylar {
/*
    foo://user@example.com:8042/over/there?name=ferret#nose
    \_/   \___________________/\_________/ \_________/ \__/
     |              |              |            |        |
    scheme      authority         path        query   fragment
     |   __________________________|__
    / \ /                             \
    urn:example:animal:ferret:nose

    authority   = [ userinfo "@" ] host [ ":" port ]
*/

class Uri {
 public:
  typedef std::shared_ptr<Uri> ptr;

  static Uri::ptr Create(const std::string& uri);
  Uri();

  const std::string& getScheme() const { return m_scheme; }
  const std::string& getUserinfo() const { return m_userinfo; }
  const std::string& getHost() const { return m_host; }
  const std::string& getPath() const;
  const std::string& getQuery() const { return m_query; }
  const std::string& getFragment() const { return m_fragment; }
  uint16_t getPort() const;

  void setScheme(std::string scheme) { m_scheme = scheme; }
  void setUserinfo(std::string userinfo) { m_userinfo = userinfo; }
  void setHost(std::string host) { m_host = host; }
  void setPath(std::string path) { m_path = path; }
  void setQuery(std::string query) { m_query = query; }
  void setFragment(std::string fragment) { m_fragment = fragment; }
  void setPort(uint16_t port) { m_port = port; }

  std::ostream& dump(std::ostream& os) const;
  std::string toString() const;
  Address::ptr createAddress() const;

 private:
  bool isDefaultPort() const;

 private:
  std::string m_scheme;
  std::string m_userinfo;
  std::string m_host;
  std::string m_path;
  std::string m_query;
  std::string m_fragment;
  uint16_t m_port;
};
}  // namespace sylar

#endif