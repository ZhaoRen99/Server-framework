#include <execinfo.h>

#include "util.h"
#include "log.h"
#include "fiber.h"

namespace sylar {

sylar::Logger::ptr g_logger = SYLAR_LOG_NAME("system");

pid_t GetThreadId() {
    return syscall(SYS_gettid);
}

uint32_t GetFiberId() {
    return sylar::Fiber::GetFiberId();
}

void Backtrace(std::vector<std::string> &bt, int size, int skip) {
    void** array = (void**)malloc(sizeof(void*) * size);
    // ����ջ֡���ݸ���
    size_t s = ::backtrace(array, size);
    // ����backtrace()������ȡ�ĵ�ַתΪ������Щ��ַ���ַ������顣
    // backtrace_symbols ��������ֵ��һ���ַ���ָ��
    char** strings = backtrace_symbols(array, size);
    if (strings == NULL) {
        SYLAR_LOG_ERROR(g_logger) << "backtrace_symbols error";
        free(array);
        free(strings);
        
        return;
    }

    for (size_t i = skip; i < s; ++i) {
        bt.push_back(strings[i]);
    }
    free(strings);
    free(array);
}

std::string BacktraceToString(int size, int skip, const std::string& perfix) {
    std::vector<std::string> bt;
    Backtrace(bt, size, skip);
    std::stringstream ss;
    for (size_t i = 0; i < bt.size(); ++i) {
        ss << perfix << bt[i] << std::endl;
    }
    return ss.str();
}
}