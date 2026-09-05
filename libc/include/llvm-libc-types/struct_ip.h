//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Definition of struct ip.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_TYPES_STRUCT_IP_H
#define LLVM_LIBC_TYPES_STRUCT_IP_H

#include "../llvm-libc-macros/endian-macros.h"
#include "../llvm-libc-macros/stdint-macros.h"
#include "struct_in_addr.h"

// The same header under its BSD names, which is what code carried over
// from there expects.
struct ip {
#if __BYTE_ORDER == __LITTLE_ENDIAN
  unsigned int ip_hl : 4; // Header length, in units of four octets.
  unsigned int ip_v : 4;  // Version.
#else
  unsigned int ip_v : 4;
  unsigned int ip_hl : 4;
#endif
  uint8_t ip_tos;         // Type of service.
  unsigned short ip_len;  // Total length.
  unsigned short ip_id;   // Identification.
  unsigned short ip_off;  // Fragment offset, and the three flags above it.
#define IP_RF 0x8000      // Reserved.
#define IP_DF 0x4000      // Do not fragment.
#define IP_MF 0x2000      // More fragments follow.
#define IP_OFFMASK 0x1fff // The offset itself.
  uint8_t ip_ttl;         // Time to live.
  uint8_t ip_p;           // What it carries.
  unsigned short ip_sum;  // Checksum.
  struct in_addr ip_src, ip_dst;
};

#endif // LLVM_LIBC_TYPES_STRUCT_IP_H
