//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Definition of struct ether_arp.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_TYPES_STRUCT_ETHER_ARP_H
#define LLVM_LIBC_TYPES_STRUCT_ETHER_ARP_H

#include "../llvm-libc-macros/net-ethernet-macros.h"
#include "../llvm-libc-macros/stdint-macros.h"
#include "struct_arphdr.h"

// An ARP message over Ethernet, where the address lengths are known and so
// the four of them can be named.
struct ether_arp {
  struct arphdr ea_hdr;
  uint8_t arp_sha[ETH_ALEN]; // Sender hardware address.
  uint8_t arp_spa[4];        // Sender protocol address.
  uint8_t arp_tha[ETH_ALEN]; // Target hardware address.
  uint8_t arp_tpa[4];        // Target protocol address.
};

#define arp_hrd ea_hdr.ar_hrd
#define arp_pro ea_hdr.ar_pro
#define arp_hln ea_hdr.ar_hln
#define arp_pln ea_hdr.ar_pln
#define arp_op ea_hdr.ar_op

#endif // LLVM_LIBC_TYPES_STRUCT_ETHER_ARP_H
