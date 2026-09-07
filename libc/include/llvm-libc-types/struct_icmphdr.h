//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Definition of struct icmphdr.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_TYPES_STRUCT_ICMPHDR_H
#define LLVM_LIBC_TYPES_STRUCT_ICMPHDR_H

#include "../llvm-libc-macros/stdint-macros.h"

/// The eight bytes an ICMP message begins with. What the last four hold
/// depends on which kind of message it is.
struct icmphdr {
  /// Which kind of message this is.
  uint8_t type;
  /// What kind of that kind.
  uint8_t code;
  uint16_t checksum;
  union {
    /// Names the exchange an echo and its reply belong to.
    struct {
      uint16_t id;
      uint16_t sequence;
    } echo;
    /// The router to use instead, in a redirect.
    uint32_t gateway;
    /// How large a datagram the next hop will carry, where one was too big.
    struct {
      uint16_t __unused;
      uint16_t mtu;
    } frag;
  } un;
};

#endif // LLVM_LIBC_TYPES_STRUCT_ICMPHDR_H
