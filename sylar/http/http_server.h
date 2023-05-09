#ifndef __SYLAR_HTTP_SERVER_H__
#define __SYLAR_HTTP_SERVER_H__

#include "../tcp_server.h"
#include "http_session.h"

namespace sylar {
namespace http{

class HttpServer : public TcpServer {
public:
    typedef std::shared_ptr<HttpServer> ptr;
    HttpServer(bool keepalive = false
        , sylar::IOManager* worker = sylar::IOManager::GetThis()
        , sylar::IOManager* accept_worker = sylar::IOManager::GetThis());
    
protected:
    void handleClient(Socket::ptr client) override;

private:
    bool m_isKeepalive;
};

}
}

#endif