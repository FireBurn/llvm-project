//===-- Definition of struct sockaddr_ll ----------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_TYPES_STRUCT_SOCKADDR_LL_H
#define LLVM_LIBC_TYPES_STRUCT_SOCKADDR_LL_H

#include "../llvm-libc-macros/stdint-macros.h"

// How a socket that speaks straight to a network device names an address:
// which device, what is carried over it, and the hardware address itself.
struct sockaddr_ll {
  unsigned short sll_family;
  uint16_t sll_protocol; // In the order the wire uses.
  int sll_ifindex;
  unsigned short sll_hatype;
  unsigned char sll_pkttype;
  unsigned char sll_halen;
  unsigned char sll_addr[8];
};

#endif // LLVM_LIBC_TYPES_STRUCT_SOCKADDR_LL_H
