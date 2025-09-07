/*
 * @Author: wangzhaoren <wangzhaoren99@163.com>
 * @Date: 2025-09-05 15:19:43
 * @LastEditors: wangzhaoren <wangzhaoren99@163.com>
 * @LastEditTime: 2025-09-06 16:04:10
 * @FilePath: /Server-framework/source/include/sylar/env/env.h
 * @Description: 
 * 
 * Copyright (c) 2025 by wangzhaoren, All Rights Reserved. 
 */
#ifndef __SYLAR_ENV_H__
#define __SYLAR_ENV_H__

#include <stdlib.h>

#include <map>
#include <memory>
#include <vector>

#include "sylar/common/singleton.h"
#include "sylar/io/mutex.h"

namespace sylar {

class Env {
 public:
  typedef RWMutex RWMutexType;

  bool init(int argc, char** argv);

  void add(const std::string& key, const std::string& value);
  bool has(const std::string& key);
  void del(const std::string& key);
  std::string get(const std::string& key,
                  const std::string& default_value = "");

  void addHelp(const std::string& key, const std::string& desc);
  void removeHelp(const std::string& key);
  void printHelp();

  const std::string& getExe() const { return m_exe; }
  const std::string& getCwd() const { return m_cwd; }

  bool setEnv(const std::string& key, const std::string& val);
  std::string getEnv(const std::string& key,
                     const std::string& default_val = "");

  std::string getAbolutePath(const std::string& path) const;

 private:
  RWMutexType m_mutex;
  std::map<std::string, std::string> m_args;
  std::vector<std::pair<std::string, std::string> > m_helps;

  std::string m_program;
  std::string m_exe;
  std::string m_cwd;
};

typedef Singleton<Env> EnvMgr;

}  // namespace sylar

#endif