//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Definition of struct ifconf.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_TYPES_STRUCT_IFCONF_H
#define LLVM_LIBC_TYPES_STRUCT_IFCONF_H

#include "struct_ifreq.h"

// What the SIOCGIFCONF ioctl fills in. On the way in ifc_len is the room in
// the block; on the way out it is how much of it was used.
struct ifconf {
  int ifc_len;
  union {
    char *ifcu_buf;
    struct ifreq *ifcu_req;
  } ifc_ifcu;
};

// The short names everything reaches for, spelled the way glibc and the
// kernel's own <linux/if.h> spell them, so that code naming either the
// union or the field through it finds what it expects.
#define ifc_buf ifc_ifcu.ifcu_buf
#define ifc_req ifc_ifcu.ifcu_req

#endif // LLVM_LIBC_TYPES_STRUCT_IFCONF_H
