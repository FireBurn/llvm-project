//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Definition of struct ipv6_mreq.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_TYPES_STRUCT_IPV6_MREQ_H
#define LLVM_LIBC_TYPES_STRUCT_IPV6_MREQ_H

#include "../llvm-libc-macros/uapi-compat-macros.h"

#include "struct_in6_addr.h"

// The kernel defines this too, in <linux/in6.h>. Leaving it to the
// kernel is right only where that header has been read and its
// guard says the definition is the kernel's.
#if !(defined(__UAPI_DEF_IPV6_MREQ) && __UAPI_DEF_IPV6_MREQ &&                 \
      defined(_LINUX_IN6_H))
struct ipv6_mreq {
  struct in6_addr ipv6mr_multiaddr;
  unsigned int ipv6mr_interface;
};

// Say the definition here is the one, so <linux/in6.h> skips its own if it
// is read after this.
#undef __UAPI_DEF_IPV6_MREQ
#define __UAPI_DEF_IPV6_MREQ 0
#endif

#endif // LLVM_LIBC_TYPES_STRUCT_IPV6_MREQ_H
