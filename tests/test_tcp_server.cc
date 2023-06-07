#include "../sylar/sylar.h"

static SYLAR__ROOT__LOG(g_logger);

void run() {
    auto addr = sylar::Address::LookupAny("0.0.0.0:8033");
    // auto addr2 = sylar::UinxAddress::ptr(new sylar::UinxAddress("/tmp/unix_addr"));
    std::vector<sylar::Address::ptr> addrs;
    addrs.push_back(addr);
    // addrs.push_back(addr2);

    sylar::TcpServer::ptr tcp_server(new sylar::TcpServer);
    std::vector<sylar::Address::ptr>fails;
    
    while (!tcp_server->bind(addrs, fails)) {
        sleep(2);
    }
    tcp_server->start();

}

int main(int argc, char** argv) {
    sylar::IOManager iom(2);
    g_logger->setLevel(sylar::LogLevel::INFO);
    iom.schedule(run);
    return 0;
}