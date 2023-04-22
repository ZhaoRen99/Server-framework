#include "../sylar/sylar.h"

static sylar::Logger::ptr g_logger = SYLAR_LOG_ROOT();

void test_fiber() {
    static int count = 5;
    SYLAR_LOG_INFO(g_logger) << "---test in fiber---" << count;

    sleep(1);

    if (--count > 0) {
        sylar::Scheduler::GetThis()->schedule(&test_fiber, sylar::GetThreadId());
    }
}

int main(int argc, char** argv) {
    SYLAR_LOG_INFO(g_logger) << "main start";

    sylar::Scheduler sc(3, true, "sc1");
    sc.start();
    SYLAR_LOG_INFO(g_logger) << "schedule";
    sc.schedule(&test_fiber);
    sc.stop();

    SYLAR_LOG_INFO(g_logger) << "main end";
    return 0;
}