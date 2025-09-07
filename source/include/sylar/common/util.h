/*
 * @Author: wangzhaoren <wangzhaoren@airia.cn>
 * @Date: 2025-09-05 15:19:43
 * @LastEditors: wangzhaoren <wangzhaoren99@163.com>
 * @LastEditTime: 2025-09-06 16:03:58
 * @FilePath: /Server-framework/source/include/sylar/common/util.h
 * @Description:
 *
 * Copyright (c) 2025 by wangzhaoren, All Rights Reserved.
 */
#ifndef __SYLAR_UTIL_H__
#define __SYLAR_UTIL_H__

#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/syscall.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#include <iostream>
#include <string>
#include <vector>

namespace sylar {

pid_t GetThreadId();  // 获取线程Id
uint32_t GetFiberId();

void Backtrace(std::vector<std::string>& bt, int size = 64, int skip = 1);

std::string BacktraceToString(int size = 64, int skip = 2,
                              const std::string& perfix = "");

// Time ms
uint64_t GetCurrentMS();
uint64_t GetCurrentUS();

std::string Time2Str(time_t ts = time(0),
                     const std::string& format = "%Y-%m-%d %H:%M:%S");
time_t Str2Time(const char* str, const char* format = "%Y-%m-%d %H:%M:%S");

class FSUtil {
 public:
  static void ListAllFile(std::vector<std::string>& files,
                          const std::string& path, const std::string& subfix);
  static bool Mkdir(const std::string& dirname);
  static bool IsRunningPidfile(const std::string& pidfile);
  static bool Rm(const std::string& path);
  static bool Mv(const std::string& from, const std::string& to);
  static bool Realpath(const std::string& path, std::string& rpath);
  static bool Symlink(const std::string& frm, const std::string& to);
  static bool Unlink(const std::string& filename, bool exist = false);
  static std::string Dirname(const std::string& filename);
  static std::string Basename(const std::string& filename);
  static bool OpenForRead(std::ifstream& ifs, const std::string& filename,
                          std::ios_base::openmode mode);
  static bool OpenForWrite(std::ofstream& ofs, const std::string& filename,
                           std::ios_base::openmode mode);
};

class StringUtil {
 public:
  static std::string Format(const char* fmt, ...);
  static std::string Formatv(const char* fmt, va_list ap);

  static std::string UrlEncode(const std::string& str,
                               bool space_as_plus = true);
  static std::string UrlDecode(const std::string& str,
                               bool space_as_plus = true);

  static std::string Trim(const std::string& str,
                          const std::string& delimit = " \t\r\n");
  static std::string TrimLeft(const std::string& str,
                              const std::string& delimit = " \t\r\n");
  static std::string TrimRight(const std::string& str,
                               const std::string& delimit = " \t\r\n");

  static std::string WStringToString(const std::wstring& ws);
  static std::wstring StringToWString(const std::string& s);
};

}  // namespace sylar

#endif