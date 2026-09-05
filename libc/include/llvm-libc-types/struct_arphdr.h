//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Definition of struct arphdr.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_TYPES_STRUCT_ARPHDR_H
#define LLVM_LIBC_TYPES_STRUCT_ARPHDR_H

// The fixed part of an ARP message. The four addresses which follow are of
// the lengths this names, so they cannot be members.
struct arphdr {
  unsigned short int ar_hrd; // What kind of hardware address.
  unsigned short int ar_pro; // What kind of protocol address.
  unsigned char ar_hln;      // How long the hardware ones are.
  unsigned char ar_pln;      // How long the protocol ones are.
  unsigned short int ar_op;  // Which of the operations it is.
};

#endif // LLVM_LIBC_TYPES_STRUCT_ARPHDR_H
