#include "../sylar/tcp_server.h"
#include "../sylar/log.h"
#include "../sylar/iomanager.h"
#include "../sylar/socket.h"
#include "../sylar/bytearray.h"
#include "../sylar/streams/socket_stream.h"

static SYLAR__ROOT__LOG(g_logger);

class EchoServer: public sylar::TcpServer {
public:
    EchoServer(int type);
    void handleClient(sylar::Socket::ptr client) override;
    
private:
    int m_type = 0;
};


EchoServer::EchoServer(int type) 
    :m_type(type){
    
}
// void EchoServer::handleClient(sylar::Socket::ptr client) {
//     SYLAR_LOG_INFO(g_logger) << "handllClient " << *client;
//     sylar::ByteArray::ptr ba(new sylar::ByteArray);
//     while (true) {
//         ba->clear();
//         std::vector<iovec> iovs;
//         ba->getWriteBuffers(iovs, 1024);
//         SYLAR_LOG_INFO(g_logger) << "&iovs[0]: " << &iovs[0] <<" iovs.size(): " << iovs.size();
//         int rt = client->recv(&iovs[0], iovs.size());
//         if (rt == 0) {
//             SYLAR_LOG_INFO(g_logger) << "client close: " << *client;
//             break;
//         } else if (rt < 0) {
//             SYLAR_LOG_INFO(g_logger) << "client error rt = " << rt
//                 << " errno = " << errno << " strerr = " << strerror(errno);
//             break;
//         }
//         ba->setPosition(ba->getPosition() + rt);
//         ba->setPosition(0);
        
//         if (m_type == 1) {  //text
//             SYLAR_LOG_INFO(g_logger) << "\n" << ba->toString();
//         } else {
//             SYLAR_LOG_INFO(g_logger) << "\n" << ba->toHexString();
//         }
//     }
// }

void EchoServer::handleClient(sylar::Socket::ptr client) {
    SYLAR_LOG_INFO(g_logger) << "handllClient " << *client;
    sylar::SocketStream::ptr tcp(new sylar::SocketStream(client));
    sylar::ByteArray::ptr ba(new sylar::ByteArray);
    while (true) {
        SYLAR_LOG_INFO(g_logger) << "==== while ====";
        ba->clear();
        int rt = tcp->read(ba, 1024);
        SYLAR_LOG_INFO(g_logger) << "read rt = " << rt;

        if (rt == 0) {
            SYLAR_LOG_INFO(g_logger) << "client close: " << *client;
            break;
        } else if (rt < 0) {
            SYLAR_LOG_INFO(g_logger) << "client error rt = " << rt
                << " errno = " << errno << " strerr = " << strerror(errno);
            break;
        }
        
        ba->setPosition(0);
        if (m_type == 1) {  //text
            SYLAR_LOG_INFO(g_logger) << "\n" << ba->toString();
        } else {
            SYLAR_LOG_INFO(g_logger) << "\n" << ba->toHexString();
        }
    }
}

int type = 1;

void run() {
    EchoServer::ptr es(new EchoServer(type));
    auto addr = sylar::Address::LookupAny("0.0.0.0:8020");
    while (!es->bind(addr)) {
        sleep(2);
    }
    es->start();
}

int main(int argc, char** argv) {
    if (argc < 2 || (strcmp(argv[1], "-b") && strcmp(argv[1], "-t"))) {
        SYLAR_LOG_INFO(g_logger) << "used as[" << argv[0] << "-t] or [" << argv[0] << "-b]";
        return 0;
    }

    if (!strcmp(argv[1], "-b")) {
        type = 2;
    }

    sylar::IOManager iom(2);
    g_logger->setLevel(sylar::LogLevel::INFO);
    iom.schedule(run);
    
    return 0;
}