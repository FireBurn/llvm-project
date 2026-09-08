//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Definition of struct in6_pktinfo.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_TYPES_STRUCT_IN6_PKTINFO_H
#define LLVM_LIBC_TYPES_STRUCT_IN6_PKTINFO_H

#include "../llvm-libc-macros/uapi-compat-macros.h"

#include "struct_in6_addr.h"

// The kernel defines this too, in <linux/in6.h>. Leaving it to the
// kernel is right only where that header has been read and its
// guard says the definition is the kernel's.
#if !(defined(__UAPI_DEF_IN6_PKTINFO) && __UAPI_DEF_IN6_PKTINFO &&             \
      defined(_LINUX_IN6_H))
// The IPv6 form of the same: the address and the interface.
struct in6_pktinfo {
  struct in6_addr ipi6_addr;
  unsigned int ipi6_ifindex;
};

// Say the definition here is the one, so <linux/in6.h> skips its own if it
// is read after this.
#undef __UAPI_DEF_IN6_PKTINFO
#define __UAPI_DEF_IN6_PKTINFO 0
#endif

#endif // LLVM_LIBC_TYPES_STRUCT_IN6_PKTINFO_H
