//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Definition of struct ip6_mtuinfo.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_TYPES_STRUCT_IP6_MTUINFO_H
#define LLVM_LIBC_TYPES_STRUCT_IP6_MTUINFO_H

#include "../llvm-libc-macros/uapi-compat-macros.h"

#include "../llvm-libc-macros/stdint-macros.h"
#include "struct_sockaddr_in6.h"

// The kernel defines this too, in <linux/in6.h>. Leaving it to the
// kernel is right only where that header has been read and its
// guard says the definition is the kernel's.
#if !(defined(__UAPI_DEF_IP6_MTUINFO) && __UAPI_DEF_IP6_MTUINFO &&             \
      defined(_LINUX_IN6_H))
// What IPV6_PATHMTU reports: the path's largest packet, in host order,
// and where that path leads.
struct ip6_mtuinfo {
  struct sockaddr_in6 ip6m_addr;
  uint32_t ip6m_mtu;
};

// Say the definition here is the one, so <linux/in6.h> skips its own if it
// is read after this.
#undef __UAPI_DEF_IP6_MTUINFO
#define __UAPI_DEF_IP6_MTUINFO 0
#endif

#endif // LLVM_LIBC_TYPES_STRUCT_IP6_MTUINFO_H
