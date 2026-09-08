//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Definition of struct ifreq.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_TYPES_STRUCT_IFREQ_H
#define LLVM_LIBC_TYPES_STRUCT_IFREQ_H

#include "../llvm-libc-macros/uapi-compat-macros.h"

#include "../llvm-libc-macros/net-if-macros.h"
#include "struct_ifmap.h"
#include "struct_sockaddr.h"

// The kernel defines this too, in <linux/if.h>. Leaving it to the
// kernel is right only where that header has been read and its
// guard says the definition is the kernel's.
#if !(defined(__UAPI_DEF_IF_IFREQ) && __UAPI_DEF_IF_IFREQ &&                   \
      defined(_LINUX_IF_H))
struct ifreq {
  char ifr_name[IF_NAMESIZE];
  __extension__ union {
    struct sockaddr ifr_hwaddr;
    struct sockaddr ifr_addr;
    struct sockaddr ifr_dstaddr;
    struct sockaddr ifr_broadaddr;
    struct sockaddr ifr_netmask;
    short int ifr_flags;
    int ifr_metric;
    int ifr_mtu;
    int ifr_ifindex;
    int ifr_bandwidth;
    int ifr_qlen;
    struct ifmap ifr_map;
    char ifr_newname[IF_NAMESIZE];
    char ifr_slave[IF_NAMESIZE];
    char *ifr_data;
  };
};

// Say the definition here is the one, so <linux/if.h> skips its own if it
// is read after this.
#undef __UAPI_DEF_IF_IFREQ
#define __UAPI_DEF_IF_IFREQ 0
#endif

#endif // LLVM_LIBC_TYPES_STRUCT_IFREQ_H
