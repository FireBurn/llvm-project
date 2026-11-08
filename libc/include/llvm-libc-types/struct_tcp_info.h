//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Definition of struct tcp_info.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_TYPES_STRUCT_TCP_INFO_H
#define LLVM_LIBC_TYPES_STRUCT_TCP_INFO_H

#if defined(__linux__)
// bionic's <netinet/tcp.h> is the kernel's <linux/tcp.h>, and Android programs
// include both, so the kernel's definition stands in for ours there.
#if defined(__ANDROID__) && defined(__has_include)
#if __has_include(<linux/tcp.h>)
#include <linux/tcp.h>
#endif
#endif
#if defined(__ANDROID__) &&                                                    \
    (defined(_UAPI_LINUX_TCP_H) || defined(_LINUX_TCP_H))
#define __LLVM_LIBC_KERNEL_TCP_H 1
#endif
#ifndef __LLVM_LIBC_KERNEL_TCP_H
#include "linux/struct_tcp_info.h"
#endif
#endif

#endif // LLVM_LIBC_TYPES_STRUCT_TCP_INFO_H
