#ifndef __SYLAR_UTIL_H__
#define __SYLAR_UTIL_H__

#include <pthread.h>
#include <sys/types.h>
#include <sys/syscall.h> 
#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <string>
#include <vector>

namespace sylar {

pid_t GetThreadId();    //获取线程Id
uint32_t GetFiberId();

void Backtrace(std::vector<std::string> &bt, int size, int skip = 1);

std::string BacktraceToString(int size, int skip = 2, const std::string& perfix = "");

}

#endif