//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Definition of struct igmp
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_TYPES_STRUCT_IGMP_H
#define LLVM_LIBC_TYPES_STRUCT_IGMP_H

#include "../llvm-libc-macros/stdint-macros.h"
#include "struct_in_addr.h"

// An IGMP message.
struct igmp {
  uint8_t igmp_type;         // Type.
  uint8_t igmp_code;         // Routing code.
  uint16_t igmp_cksum;       // Checksum.
  struct in_addr igmp_group; // Group address.
};

#endif // LLVM_LIBC_TYPES_STRUCT_IGMP_H
