//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Definition of struct ip_mreqn.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_TYPES_STRUCT_IP_MREQN_H
#define LLVM_LIBC_TYPES_STRUCT_IP_MREQN_H

#include "../llvm-libc-macros/uapi-compat-macros.h"

#include "struct_in_addr.h"

// The kernel defines this too, in <linux/in.h>. Leaving it to the
// kernel is right only where that header has been read and its
// guard says the definition is the kernel's.
#if !(defined(__UAPI_DEF_IP_MREQ) && __UAPI_DEF_IP_MREQ && defined(_LINUX_IN_H))
struct ip_mreqn {
  struct in_addr imr_multiaddr;
  struct in_addr imr_address;
  int imr_ifindex;
};

// Say the definition here is the one, so <linux/in.h> skips its own if it
// is read after this.
#undef __UAPI_DEF_IP_MREQ
#define __UAPI_DEF_IP_MREQ 0
#endif

#endif // LLVM_LIBC_TYPES_STRUCT_IP_MREQN_H
