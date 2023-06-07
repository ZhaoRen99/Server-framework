#include "../sylar/http/http_server.h"

static SYLAR__ROOT__LOG(g_logger);

void run(){
    sylar::http::HttpServer::ptr server(new sylar::http::HttpServer(true));
    sylar::Address::ptr addr1 = sylar::Address::LookupAnyIPAddress("0.0.0.0:8020");
    sylar::Address::ptr addr2 = sylar::Address::LookupAnyIPAddress("127.0.0.1:8030");
    std::vector<sylar::Address::ptr> addr, fail;
    // addr.push_back(addr1);
    addr.push_back(addr2);
    
    server->setName("zhaoren/1.0.0");
    while (!server->bind(addr, fail)) {
        sleep(2);
    }
    auto sd = server->getServletDispatch();
    sd->addServlet("/sylar/xx", [](sylar::http::HttpRequest::ptr req
        , sylar::http::HttpResponse::ptr rsp
        , sylar::http::HttpSession::ptr session) {
            rsp->setBody(req->toString());
            return 0;
        });

    sd->addGlobServlet("/sylar/*", [](sylar::http::HttpRequest::ptr req
        , sylar::http::HttpResponse::ptr rsp
        , sylar::http::HttpSession::ptr session) {
            rsp->setBody("Glob:\r\n" + req->toString() + "\r\n" + rsp->toString());
            return 0;
        });
    server->start();
}

int main(int argc, char** argv) {
    g_logger->setLevel(sylar::LogLevel::INFO);
    sylar::IOManager iom(2);
    iom.schedule(run);
    return 0;
}