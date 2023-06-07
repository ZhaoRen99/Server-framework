#include "../sylar/sylar.h"
#include "../sylar/iomanager.h"

#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

static sylar::Logger::ptr g_logger = SYLAR_LOG_ROOT();
static SYLAR__SYSTEM__LOG(s_logger);
int sock = 0;

void test_fiber() {
    SYLAR_LOG_INFO(g_logger) << "test_fiber";

    sock = socket(AF_INET, SOCK_STREAM, 0);
    fcntl(sock, F_SETFL, O_NONBLOCK);

    sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(80);
    inet_pton(AF_INET, "112.80.248.75", &addr.sin_addr.s_addr);

    if (!connect(sock, (const sockaddr*)&addr, sizeof(addr))) {
    } else if(errno == EINPROGRESS) {
        SYLAR_LOG_INFO(g_logger) << "add event errno=" << errno << " " << strerror(errno);

        sylar::IOManager::GetThis()->addEvent(sock, sylar::IOManager::READ, [](){
            SYLAR_LOG_INFO(g_logger) << "read callback";
            char temp[1000];
            int rt = read(sock, temp, 1000);
            if (rt >= 0) {
                std::string ans(temp, rt);
                SYLAR_LOG_INFO(g_logger) << "read:\n["<< ans << "]";
            } else {
                SYLAR_LOG_INFO(g_logger) << "read rt = " << rt;
            }
            });
        sylar::IOManager::GetThis()->addEvent(sock, sylar::IOManager::WRITE, [](){
            SYLAR_LOG_INFO(g_logger) << "write callback";
            int rt = write(sock, "GET / HTTP/1.1\r\ncontent-length: 0\r\n\r\n",38);
            SYLAR_LOG_INFO(g_logger) << "write rt = " << rt;
            // sylar::IOManager::GetThis()->cancelEvent(sock, sylar::IOManager::WRITE);
            // close(sock);
            });

    } else {
        SYLAR_LOG_INFO(g_logger) << "else " << errno << " " << strerror(errno);
    }
    SYLAR_LOG_INFO(g_logger) << "test_fiber end";
}

void test01() {
    sylar::IOManager iom(2, true, "IOM");
    iom.schedule(test_fiber);
}

sylar::Timer::ptr s_timer;
void test_timer() {
    sylar::IOManager iom(2, true, "IOM2");
    s_timer = iom.addTimer(1000, []() {
        static int i = 0;
        SYLAR_LOG_INFO(g_logger) << "hello timer i = " << i;
        if (++i == 5) {
            s_timer->reset(2000, true);
            // s_timer->cancel();
        }
        if (i == 10) {
            s_timer->cancel();
        }
    }, true);
}

int main(int argc, char** argv) {
    // g_logger->setLevel(sylar::LogLevel::INFO);
    // test01();
    test_timer();
    return 0;
}