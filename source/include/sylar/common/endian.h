/*
 * @Author: wangzhaoren <wangzhaoren99@163.com>
 * @Date: 2025-09-05 15:19:43
 * @LastEditors: wangzhaoren <wangzhaoren99@163.com>
 * @LastEditTime: 2025-09-06 15:38:22
 * @FilePath: /Server-framework/source/include/sylar/common/endian.h
 * @Description: 
 * 
 * Copyright (c) 2025 by wangzhaoren, All Rights Reserved. 
 */

#ifndef __SYLAR_ENDIAN_H__
#define __SYLAR_ENDIAN_H__

#define SYLAR_LITTLE_ENDIAN 1
#define SYLAR_BIG_ENDIAN 2

#include <byteswap.h>
#include <stdint.h>

#include <type_traits>

namespace sylar {

template <class T>
typename std::enable_if<sizeof(T) == sizeof(uint64_t), T>::type byteswap(
    T value) {
  return (T)bswap_64((uint64_t)value);
}

template <class T>
typename std::enable_if<sizeof(T) == sizeof(uint32_t), T>::type byteswap(
    T value) {
  return (T)bswap_32((uint32_t)value);
}

template <class T>
typename std::enable_if<sizeof(T) == sizeof(uint16_t), T>::type byteswap(
    T value) {
  return (T)bswap_16((uint16_t)value);
}

#if BYTE_ORDER == BIG_ENDIAN
#define SYLAR_BYTE_ORDER SYLAR_BIG_ENDIAN
#else
#define SYLAR_BYTE_ORDER SYLAR_LITTLE_ENDIAN
#endif

#if SYLAR_BYTE_ORDER == SYLAR_BIG_ENDIAN
template <class T>
T byteswapOnLittleEndian(T t) {
  return t;
}

template <class T>
T byteswapOnBigEndian(T t) {
  return byteswap(t);
}

#else
template <class T>
T byteswapOnLittleEndian(T t) {
  return byteswap(t);
}

template <class T>
T byteswapOnBigEndian(T t) {
  return t;
}
#endif

}  // namespace sylar
#endif