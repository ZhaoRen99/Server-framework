#include "../sylar/sylar.h"
#include "../sylar/hook.h"

#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

static sylar::Logger::ptr g_logger = SYLAR_LOG_ROOT();

void test_sleep() {
    sylar::IOManager iom(1);
    iom.schedule([]() {
        sleep(2);
        // sleep(3);
        SYLAR_LOG_INFO(g_logger) << "sleep 2";
        });

    iom.schedule([]() {
        sleep(3);
        SYLAR_LOG_INFO(g_logger) << "sleep 3";
        });
    
    SYLAR_LOG_INFO(g_logger) << "test sleep";
}

void test_socket() {
    sylar::IOManager::GetThis()->schedule([]() {
            SYLAR_LOG_INFO(g_logger) << "add sleep timer";
            sleep(2);
            SYLAR_LOG_INFO(g_logger) << "sleep 3";
        });
    sylar::IOManager::GetThis()->schedule([]() {
            int sock = socket(AF_INET, SOCK_STREAM, 0);

            sockaddr_in addr;
            memset(&addr, 0, sizeof(addr));
            addr.sin_family = AF_INET;
            addr.sin_port = htons(80);
            inet_pton(AF_INET, "112.80.248.76", &addr.sin_addr.s_addr);
            SYLAR_LOG_INFO(g_logger) << "begin connect";
            int rt = connect(sock, (const sockaddr*)&addr, sizeof(addr));
            SYLAR_LOG_INFO(g_logger) << "connect rt = " << rt << ", errno = " << errno;
            if (rt) {
                return;
            }

            const char data[] = "GET / HTTP/1.0\r\n\r\n";
            rt = send(sock, data, sizeof(data), 0);
            SYLAR_LOG_INFO(g_logger) << "send rt = " << rt << ", errno = " << errno;

            if (rt <= 0) {
                return;
            }

            std::string buff;
            buff.resize(4096);

            rt = recv(sock, &buff[0], buff.size(), 0);
            SYLAR_LOG_INFO(g_logger) << "recv rt = " << rt << ", errno = " << errno;
            if (rt <= 0) {
                return;
            }

            buff.resize(rt);
            SYLAR_LOG_INFO(g_logger) << "================";
        });
    sleep(3);
    int sock = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(80);
    inet_pton(AF_INET, "112.80.248.76", &addr.sin_addr.s_addr);
    SYLAR_LOG_INFO(g_logger) << "begin connect";
    int rt = connect(sock, (const sockaddr*)&addr, sizeof(addr));
    SYLAR_LOG_INFO(g_logger) << "connect rt = " << rt << ", errno = " << errno;
    if (rt) {
        return;
    }

    const char data[] = "GET / HTTP/1.0\r\n\r\n";
    rt = send(sock, data, sizeof(data), 0);
    SYLAR_LOG_INFO(g_logger) << "send rt = " << rt << ", errno = " << errno;

    if (rt <= 0) {
        return;
    }

    std::string buff;
    buff.resize(4096);

    rt = recv(sock, &buff[0], buff.size(), 0);
    SYLAR_LOG_INFO(g_logger) << "recv rt = " << rt << ", errno = " << errno;
    if (rt <= 0) {
        return;
    }

    buff.resize(rt);
    SYLAR_LOG_INFO(g_logger) << buff;
}

void test() {
    sylar::IOManager iom(2);
    iom.schedule(test_socket);
}

int main(int argc, char** argv) {
    // g_logger->setLevel(sylar::LogLevel::INFO);
    // test_sleep();
    // test_socket();
    // test_fiber();
    
    test();
    
    return 0;
}