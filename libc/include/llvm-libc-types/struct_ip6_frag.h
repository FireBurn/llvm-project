//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Definition of struct ip6_frag.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_TYPES_STRUCT_IP6_FRAG_H
#define LLVM_LIBC_TYPES_STRUCT_IP6_FRAG_H

#include "../llvm-libc-macros/endian-macros.h"
#include "../llvm-libc-macros/stdint-macros.h"
// The fragment header.
struct ip6_frag {
  uint8_t ip6f_nxt;      // What comes next.
  uint8_t ip6f_reserved;
  uint16_t ip6f_offlg;   // Offset, reserved bits and the more flag.
  uint32_t ip6f_ident;
};

#if __BYTE_ORDER == __BIG_ENDIAN
#define IP6F_OFF_MASK 0xfff8      // The offset out of ip6f_offlg.
#define IP6F_RESERVED_MASK 0x0006 // The reserved bits.
#define IP6F_MORE_FRAG 0x0001     // More fragments follow.
#else
#define IP6F_OFF_MASK 0xf8ff
#define IP6F_RESERVED_MASK 0x0600
#define IP6F_MORE_FRAG 0x0100
#endif

#endif // LLVM_LIBC_TYPES_STRUCT_IP6_FRAG_H
