//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Definition of struct in_pktinfo.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_TYPES_STRUCT_IN_PKTINFO_H
#define LLVM_LIBC_TYPES_STRUCT_IN_PKTINFO_H

#include "../llvm-libc-macros/uapi-compat-macros.h"

#include "struct_in_addr.h"

// The kernel defines this too, in <linux/in.h>.
// Skipping it here is right only when that header has been read and
// its guard says the definition is the kernel's.
#if !(defined(__UAPI_DEF_IN_PKTINFO) && __UAPI_DEF_IN_PKTINFO &&               \
      defined(_LINUX_IN_H))
// What IP_PKTINFO reports about a datagram, or asks about one being sent:
// which interface it came in on and which of that interface's addresses it
// was sent to.
struct in_pktinfo {
  int ipi_ifindex;
  struct in_addr ipi_spec_dst;
  struct in_addr ipi_addr;
};

// Say the definition here is the one, so <linux/in.h> skips its own if it
// is read after this.
#undef __UAPI_DEF_IN_PKTINFO
#define __UAPI_DEF_IN_PKTINFO 0
#endif

#endif // LLVM_LIBC_TYPES_STRUCT_IN_PKTINFO_H
