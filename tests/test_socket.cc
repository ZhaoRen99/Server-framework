#include "../sylar/sylar.h"

static SYLAR__ROOT__LOG(g_logger);

void test_sock() {
    sylar::IPAddress::ptr addr = sylar::Address::LookupAnyIPAddress("www.baidu.com");
    if (addr) {
        SYLAR_LOG_INFO(g_logger) << "get address: " << addr->toString();
    } else {
        SYLAR_LOG_ERROR(g_logger) << "get address fail";
        return;
    }

    sylar::Socket::ptr sock = sylar::Socket::CreateTCP(addr);
    addr->setPort(80);
    if (!sock->connect(addr)) {
        SYLAR_LOG_ERROR(g_logger) << "connect " << addr->toString() << "fail";
    } else {
        SYLAR_LOG_INFO(g_logger) << "connect " << addr->toString() << " connected";
    }

    const char buffer[] = "GET / HTTP/1.1\r\n\r\n";
    int rt = sock->send(buffer, sizeof(buffer));
    if (rt < 0) {
        SYLAR_LOG_INFO(g_logger) << "send fail rt = " << rt;
        return;
    }

    std::string buffers;
    buffers.resize(4096);
    rt = sock->recv(&buffers[0], buffers.size());
    if (rt < 0) {
        SYLAR_LOG_INFO(g_logger) << "recv fail rt = " << rt;
        return;
    }
    buffers.resize(rt);
    SYLAR_LOG_INFO(g_logger) << buffers;
}

int main(int argc, char** argv) {
    sylar::IOManager iom;
    iom.schedule(&test_sock);
    return 0;
}