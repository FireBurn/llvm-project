//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Definition of struct ip6_rthdr.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_TYPES_STRUCT_IP6_RTHDR_H
#define LLVM_LIBC_TYPES_STRUCT_IP6_RTHDR_H

#include "../llvm-libc-macros/stdint-macros.h"
// The routing header. Data specific to the routing type follows it.
struct ip6_rthdr {
  uint8_t ip6r_nxt;     // What comes next.
  uint8_t ip6r_len;     // Length in units of eight octets, less the first.
  uint8_t ip6r_type;    // Routing type.
  uint8_t ip6r_segleft; // Segments left.
};

#endif // LLVM_LIBC_TYPES_STRUCT_IP6_RTHDR_H
