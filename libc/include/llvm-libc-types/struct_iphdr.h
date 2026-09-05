//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Definition of struct iphdr.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_TYPES_STRUCT_IPHDR_H
#define LLVM_LIBC_TYPES_STRUCT_IPHDR_H

#include "../llvm-libc-macros/endian-macros.h"
#include "../llvm-libc-macros/stdint-macros.h"

// The IPv4 header as it sits on the wire. The first octet holds two four
// bit fields, and which of them comes first depends on how the machine
// lays out bit fields.
struct iphdr {
#if __BYTE_ORDER == __LITTLE_ENDIAN
  unsigned int ihl : 4;
  unsigned int version : 4;
#else
  unsigned int version : 4;
  unsigned int ihl : 4;
#endif
  uint8_t tos;
  uint16_t tot_len;
  uint16_t id;
  uint16_t frag_off;
  uint8_t ttl;
  uint8_t protocol;
  uint16_t check;
  uint32_t saddr;
  uint32_t daddr;
  // The options follow.
};

#endif // LLVM_LIBC_TYPES_STRUCT_IPHDR_H
