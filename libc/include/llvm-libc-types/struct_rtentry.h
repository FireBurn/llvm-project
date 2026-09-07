//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Definition of struct rtentry.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_TYPES_STRUCT_RTENTRY_H
#define LLVM_LIBC_TYPES_STRUCT_RTENTRY_H

#include "struct_sockaddr.h"

/// One route, as the ioctls which add and remove them are handed it. The
/// members named pad are where the structure once held something else and
/// are kept so that the rest stay where they are.
struct rtentry {
  unsigned long int rt_pad1;
  /// Where the route leads.
  struct sockaddr rt_dst;
  /// The gateway to send by, where the route names one.
  struct sockaddr rt_gateway;
  /// Which addresses of the destination the route stands for.
  struct sockaddr rt_genmask;
  /// What is known about the route, as the RTF_ flags.
  unsigned short int rt_flags;
  short int rt_pad2;
  unsigned long int rt_pad3;
  unsigned char rt_tos;
  unsigned char rt_class;
#if defined(__LP64__) || defined(_LP64)
  short int rt_pad4[3];
#else
  short int rt_pad4;
#endif
  /// How far away the destination is, counted one higher than it is.
  short int rt_metric;
  /// The interface to send by, where the route names one.
  char *rt_dev;
  unsigned long int rt_mtu;
// The name this went by before a route could state a window as well.
#define rt_mss rt_mtu
  unsigned long int rt_window;
  /// The round trip time to start from.
  unsigned short int rt_irtt;
};

#endif // LLVM_LIBC_TYPES_STRUCT_RTENTRY_H
