//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Definition of struct packet_mreq
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_TYPES_STRUCT_PACKET_MREQ_H
#define LLVM_LIBC_TYPES_STRUCT_PACKET_MREQ_H

#ifdef __ANDROID__
// Android code reads the kernel's definition through <linux/if_packet.h> as
// well; two of them in one translation unit do not compile.
#include <linux/if_packet.h>
#else
// A request for a device to receive frames it would otherwise drop: which
// device, which of the PACKET_MR_* modes, and the address the mode needs.
struct packet_mreq {
  int mr_ifindex;
  unsigned short mr_type;
  unsigned short mr_alen;
  unsigned char mr_address[8];
};
#endif // __ANDROID__

#endif // LLVM_LIBC_TYPES_STRUCT_PACKET_MREQ_H
