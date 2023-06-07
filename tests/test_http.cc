#include "../sylar/sylar.h"

void test_request() {
    sylar::http::HttpRequest::ptr req(new sylar::http::HttpRequest);
    req->setHeader("host", "www.baidu.com");
    req->setBody("hello sylar");
    req->dump(std::cout) << std::endl;
}

void test_response() {
    sylar::http::HttpResponse::ptr rep(new sylar::http::HttpResponse);
    rep->setHeader("X-X", "sylar");
    rep->setBody("hello sylar");
    rep->setStatue((sylar::http::HttpStatus)400);
    rep->setClose(false);
    rep->setCookie("key", "100", time(0), "\\sylar", "www.sylar.top", true);
    
    rep->dump(std::cout) << std::endl;
}

int main(int argc, char** argv) {
    test_request();
    test_response();
    return 0;
}