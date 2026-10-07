//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Definition of struct ip6_ext.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_TYPES_STRUCT_IP6_EXT_H
#define LLVM_LIBC_TYPES_STRUCT_IP6_EXT_H

#include "../llvm-libc-macros/stdint-macros.h"
// The generic extension header.
struct ip6_ext {
  uint8_t ip6e_nxt; // What comes next.
  uint8_t ip6e_len; // Length in units of eight octets, less the first.
};

#endif // LLVM_LIBC_TYPES_STRUCT_IP6_EXT_H
