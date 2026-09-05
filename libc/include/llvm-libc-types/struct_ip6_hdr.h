//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Definition of struct ip6_hdr.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_TYPES_STRUCT_IP6_HDR_H
#define LLVM_LIBC_TYPES_STRUCT_IP6_HDR_H

#include "../llvm-libc-macros/stdint-macros.h"
#include "struct_in6_addr.h"

// The IPv6 header. The first word packs the version, traffic class and flow
// label together, so it is reached either as the whole word or through the
// two named pieces of it.
struct ip6_hdr {
  union {
    struct ip6_hdrctl {
      uint32_t ip6_un1_flow; // Version, traffic class and flow label.
      uint16_t ip6_un1_plen; // Payload length.
      uint8_t ip6_un1_nxt;   // What comes next.
      uint8_t ip6_un1_hlim;  // Hop limit.
    } ip6_un1;
    uint8_t ip6_un2_vfc; // The version and the top half of the class.
  } ip6_ctlun;
  struct in6_addr ip6_src;
  struct in6_addr ip6_dst;
};

#define ip6_vfc ip6_ctlun.ip6_un2_vfc
#define ip6_flow ip6_ctlun.ip6_un1.ip6_un1_flow
#define ip6_plen ip6_ctlun.ip6_un1.ip6_un1_plen
#define ip6_nxt ip6_ctlun.ip6_un1.ip6_un1_nxt
#define ip6_hlim ip6_ctlun.ip6_un1.ip6_un1_hlim
#define ip6_hops ip6_ctlun.ip6_un1.ip6_un1_hlim

#endif // LLVM_LIBC_TYPES_STRUCT_IP6_HDR_H
