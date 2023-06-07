#include "../sylar/sylar.h"

static sylar::Logger::ptr g_logger = SYLAR_LOG_ROOT();

void test_fiber() {
    static int count = 5;
    SYLAR_LOG_INFO(g_logger) << "---test in fiber---" << count;

    sylar::set_hook_enable(false);
    sleep(1);

    if (--count > 0) {
        sylar::Scheduler::GetThis()->schedule(&test_fiber);
    }
}

int main(int argc, char** argv) {
    // g_logger->setLevel(sylar::LogLevel::INFO);
    sylar::Thread::SetName("main");
    SYLAR_LOG_INFO(g_logger) << "main start";
    sylar::Scheduler sc(2, true, "work");
    sc.schedule(&test_fiber);
    sc.start();
    SYLAR_LOG_INFO(g_logger) << "schedule";
    sc.stop();

    SYLAR_LOG_INFO(g_logger) << "main end";
    return 0;
}