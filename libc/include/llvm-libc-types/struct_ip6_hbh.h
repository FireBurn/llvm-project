//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Definition of struct ip6_hbh.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_TYPES_STRUCT_IP6_HBH_H
#define LLVM_LIBC_TYPES_STRUCT_IP6_HBH_H

#include "../llvm-libc-macros/stdint-macros.h"

// The hop-by-hop options header. The options follow it.
struct ip6_hbh {
  uint8_t ip6h_nxt; // What comes next.
  uint8_t ip6h_len; // Length in units of eight octets, less the first.
};

#endif // LLVM_LIBC_TYPES_STRUCT_IP6_HBH_H
