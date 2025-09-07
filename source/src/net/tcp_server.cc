/*
 * @Author: wangzhaoren <wangzhaoren@airia.cn>
 * @Date: 2025-09-05 15:26:05
 * @LastEditors: wangzhaoren <wangzhaoren99@163.com>
 * @LastEditTime: 2025-09-06 18:14:00
 * @FilePath: /Server-framework/source/src/net/tcp_server.cc
 * @Description:
 *
 * Copyright (c) 2025 by wangzhaoren, All Rights Reserved.
 */
#include "sylar/net/tcp_server.h"

#include "sylar/config/config.h"

namespace sylar {

static SYLAR__SYSTEM__LOG(g_logger);

static sylar::ConfigVar<uint64_t>::ptr g_tcp_server_read_timeout =
    sylar::Config::Lookup("tcp_server.read_timeout", (uint64_t)(60 * 1000 * 2),
                          "tcp server read timeout");

TcpServer::TcpServer(sylar::IOManager* worker, sylar::IOManager* accept_worker)
    : m_worker(worker),
      m_acceptWorker(accept_worker),
      m_recvTimeout(g_tcp_server_read_timeout->getValue()),
      m_name("sylar/1.0.0"),
      m_isStop(true) {}

TcpServer::~TcpServer() {
  for (auto& i : m_socks) {
    i->close();
  }
  m_socks.clear();
}

bool TcpServer::bind(sylar::Address::ptr address) {
  std::vector<Address::ptr> addrs;
  std::vector<Address::ptr> fails;
  addrs.push_back(address);
  return bind(addrs, fails);
}

bool TcpServer::bind(const std::vector<Address::ptr>& addrs,
                     std::vector<Address::ptr>& fails) {
  for (auto& addr : addrs) {
    Socket::ptr sock = Socket::CreateTCP(addr);
    if (!sock->bind(addr)) {
      SYLAR_LOG_DEBUG(g_logger)
          << "bind fail errno = " << errno << "strerr = " << strerror(errno)
          << " addr = [" << addr->toString() << "]";
      fails.push_back(addr);
      continue;
    }
    if (!sock->listen()) {
      SYLAR_LOG_ERROR(g_logger)
          << "listen fali errno = " << errno << " errstr = " << strerror(errno)
          << " addr = [" << addr->toString() << "]";
      fails.push_back(addr);
      continue;
    }
    m_socks.push_back(sock);
  }
  if (!fails.empty()) {
    m_socks.clear();
    return false;
  }

  for (auto& i : m_socks) {
    SYLAR_LOG_INFO(g_logger) << "server bind success: " << *i;
  }

  return true;
}

void TcpServer::startAccept(Socket::ptr sock) {
  while (!m_isStop) {
    Socket::ptr client = sock->accept();
    if (client) {
      SYLAR_LOG_INFO(g_logger) << "accept client" << *client;
      client->setRecvTimeout(m_recvTimeout);
      // handleClient 结束之前，
      // TcpServer不能结束，shared_from_this，把自己传进去
      m_worker->schedule(
          std::bind(&TcpServer::handleClient, shared_from_this(), client));
    } else {
      SYLAR_LOG_ERROR(g_logger)
          << "accept errno = " << errno << " strerr = " << strerror(errno);
    }
  }
}

bool TcpServer::start() {
  if (!m_isStop) {
    return true;
  }
  m_isStop = false;
  for (auto& sock : m_socks) {
    m_acceptWorker->schedule(
        std::bind(&TcpServer::startAccept, shared_from_this(), sock));
  }
  return true;
}

void TcpServer::stop() {
  m_isStop = true;
  auto self = shared_from_this();
  m_acceptWorker->schedule([this, self]() {
    for (auto& sock : m_socks) {
      sock->cancelAll();
      sock->close();
    }
    m_socks.clear();
  });
}

void TcpServer::handleClient(Socket::ptr client) {
  SYLAR_LOG_DEBUG(g_logger) << "handleClient: " << *client;
}

}  // namespace sylar