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


#include "struct_in_addr.h"

// What IP_PKTINFO reports about a datagram, or asks about one being sent:
// which interface it came in on and which of that interface's addresses it
// was sent to.
struct in_pktinfo {
  int ipi_ifindex;
  struct in_addr ipi_spec_dst;
  struct in_addr ipi_addr;
};

#endif // LLVM_LIBC_TYPES_STRUCT_IN_PKTINFO_H