//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Definition of struct ifaddrs.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_TYPES_STRUCT_IFADDRS_H
#define LLVM_LIBC_TYPES_STRUCT_IFADDRS_H

#include "struct_sockaddr.h"

// One address of one interface. getifaddrs answers with a list of these, and
// an interface with several addresses appears once for each of them.
struct ifaddrs {
  struct ifaddrs *ifa_next;
  char *ifa_name;
  // The interface's flags, the same ones SIOCGIFFLAGS reports.
  unsigned int ifa_flags;
  struct sockaddr *ifa_addr;
  struct sockaddr *ifa_netmask;
  union {
    // Only one of these is meaningful, and ifa_flags says which: the
    // broadcast address where IFF_BROADCAST is set, and the other end where
    // IFF_POINTOPOINT is. Never both.
    struct sockaddr *ifu_broadaddr;
    struct sockaddr *ifu_dstaddr;
  } ifa_ifu;
  void *ifa_data;
};

#ifndef ifa_broadaddr
#define ifa_broadaddr ifa_ifu.ifu_broadaddr
#endif
#ifndef ifa_dstaddr
#define ifa_dstaddr ifa_ifu.ifu_dstaddr
#endif

#endif // LLVM_LIBC_TYPES_STRUCT_IFADDRS_H
