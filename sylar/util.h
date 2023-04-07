#ifndef __SYLAR_UTIL_H__
#define __SYLAR_UTIL_H__

#include <pthread.h>
#include <sys/types.h>
#include <sys/syscall.h> 
#include <sys/time.h>
#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <string>
#include <vector>

namespace sylar {

pid_t GetThreadId();    //获取线程Id
uint32_t GetFiberId();

void Backtrace(std::vector<std::string> &bt, int size = 64, int skip = 1);

std::string BacktraceToString(int size = 64, int skip = 2, const std::string& perfix = "");

// Time ms
uint64_t GetCurrentMS();
uint64_t GetCurrentUS();
}

#endif