#include "http_server.h"

namespace sylar {
namespace http{

static SYLAR__SYSTEM__LOG(g_logger);

HttpServer::HttpServer(bool keepalive
            , sylar::IOManager* worker
            , sylar::IOManager* accept_worker)
    :TcpServer(worker, accept_worker)
    ,m_isKeepalive(keepalive) {

}
    
void HttpServer::handleClient(Socket::ptr client) {
    sylar::http::HttpSession::ptr session(new HttpSession(client));
    do {
        auto req = session->recvRequest();
        if (!req) {
            SYLAR_LOG_WARN(g_logger) << "recv http request fail, errno = "
                << errno << " errstr = " << strerror(errno)
                << " cliet: " << *client;
            break;
        }
        HttpRespons::ptr rsp(new HttpRespons(req->getVersion()
                                , req->isClosed() || !m_isKeepalive));
        rsp->setBody("hello sylar");

        SYLAR_LOG_INFO(g_logger) << "request: " << std::endl
            << *req;
        SYLAR_LOG_INFO(g_logger) << "respons: " << std::endl
            << *rsp;

        session->sendResponse(rsp);
    } while (m_isKeepalive);
    session->close();
}

}
}