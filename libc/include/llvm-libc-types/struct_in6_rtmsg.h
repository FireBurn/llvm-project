//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Definition of struct in6_rtmsg.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_TYPES_STRUCT_IN6_RTMSG_H
#define LLVM_LIBC_TYPES_STRUCT_IN6_RTMSG_H

#include "../llvm-libc-macros/stdint-macros.h"
#include "struct_in6_addr.h"

/// One IPv6 route, as the ioctls which add and remove them are handed it.
struct in6_rtmsg {
  struct in6_addr rtmsg_dst;
  struct in6_addr rtmsg_src;
  struct in6_addr rtmsg_gateway;
  uint32_t rtmsg_type;
  uint16_t rtmsg_dst_len;
  uint16_t rtmsg_src_len;
  uint32_t rtmsg_metric;
  unsigned long int rtmsg_info;
  uint32_t rtmsg_flags;
  int rtmsg_ifindex;
};

#endif // LLVM_LIBC_TYPES_STRUCT_IN6_RTMSG_H
