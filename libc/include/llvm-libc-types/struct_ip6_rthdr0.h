//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Definition of struct ip6_rthdr0.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_TYPES_STRUCT_IP6_RTHDR0_H
#define LLVM_LIBC_TYPES_STRUCT_IP6_RTHDR0_H

#include "../llvm-libc-macros/stdint-macros.h"
#include "struct_in6_addr.h"
// The type 0 routing header, followed by up to 127 addresses.
struct ip6_rthdr0 {
  uint8_t ip6r0_nxt;
  uint8_t ip6r0_len;
  uint8_t ip6r0_type;     // Always zero.
  uint8_t ip6r0_segleft;
  uint8_t ip6r0_reserved;
  uint8_t ip6r0_slmap[3]; // Strict and loose bit map.
  struct in6_addr ip6r0_addr[];
};

#endif // LLVM_LIBC_TYPES_STRUCT_IP6_RTHDR0_H
