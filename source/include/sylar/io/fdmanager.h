/*
 * @Author: wangzhaoren <wangzhaoren99@163.com>
 * @Date: 2025-09-05 15:19:43
 * @LastEditors: wangzhaoren <wangzhaoren99@163.com>
 * @LastEditTime: 2025-09-06 16:05:36
 * @FilePath: /Server-framework/source/include/sylar/io/fdmanager.h
 * @Description:
 *
 * Copyright (c) 2025 by wangzhaoren, All Rights Reserved.
 */
#ifndef __SYLAR_FDMANAGER_H_
#define __SYLAR_FDMANAGER_H_

#include <memory>

#include "iomanager.h"
#include "mutex"
#include "sylar/common/singleton.h"

namespace sylar {

class FdCtx : std::enable_shared_from_this<FdCtx> {
 public:
  typedef std::shared_ptr<FdCtx> ptr;
  FdCtx(int fd);
  ~FdCtx();

  bool init();
  bool isInit() const { return m_isInit; }
  bool isSocket() const { return m_isSocket; }
  bool isClosed() const { return m_isClosed; }

  void setUserNonblock(bool v) { m_usrNonblock = v; }
  bool getUserNonblock() const { return m_usrNonblock; }

  void setSysNonblock(bool v) { m_sysNonblock = v; }
  bool getSysNonblock() const { return m_sysNonblock; }

  void setTimeout(int type, uint64_t v);
  uint64_t getTimeout(int type);

 private:
  bool m_isInit : 1;
  bool m_isSocket : 1;
  bool m_sysNonblock : 1;
  bool m_usrNonblock : 1;
  bool m_isClosed : 1;
  int m_fd;
  uint64_t m_recvTimeout;
  uint64_t m_sendTimeout;
  sylar::IOManager* m_iomanager;
};

class FdManager {
 public:
  typedef RWMutex RWMUtexType;
  FdManager();

  FdCtx::ptr get(int fd, bool auto_create = false);
  void del(int fd);

 private:
  RWMUtexType m_mutex;
  std::vector<FdCtx::ptr> m_datas;
};

typedef Singleton<FdManager> FdMgr;

}  // namespace sylar

#endif