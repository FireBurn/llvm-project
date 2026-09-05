//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Definition of struct ether_header.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_TYPES_STRUCT_ETHER_HEADER_H
#define LLVM_LIBC_TYPES_STRUCT_ETHER_HEADER_H

#include "../llvm-libc-macros/net-ethernet-macros.h"
#include "../llvm-libc-macros/stdint-macros.h"

// The header an Ethernet frame begins with, as it sits on the wire.
struct ether_header {
  uint8_t ether_dhost[ETH_ALEN]; // Where it is going.
  uint8_t ether_shost[ETH_ALEN]; // Where it came from.
  uint16_t ether_type;           // What it carries, in network order.
} __attribute__((__packed__));

#endif // LLVM_LIBC_TYPES_STRUCT_ETHER_HEADER_H
