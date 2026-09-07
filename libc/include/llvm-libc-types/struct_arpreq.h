//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Definition of struct arpreq.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_TYPES_STRUCT_ARPREQ_H
#define LLVM_LIBC_TYPES_STRUCT_ARPREQ_H

#include "struct_sockaddr.h"

/// One entry of the table mapping a protocol address to a hardware one, as
/// the ioctls which read and change that table pass it.
struct arpreq {
  /// The protocol address.
  struct sockaddr arp_pa;
  /// The hardware address it is at.
  struct sockaddr arp_ha;
  /// What is known about the entry, as the ATF_ flags.
  int arp_flags;
  /// Which addresses the entry stands for, where it stands for a range.
  struct sockaddr arp_netmask;
  /// The interface it was learned on.
  char arp_dev[16];
};

#endif // LLVM_LIBC_TYPES_STRUCT_ARPREQ_H
