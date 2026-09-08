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
  __extension__ union {
    char *ifc_buf;
    struct ifreq *ifc_req;
  };
};

#endif // LLVM_LIBC_TYPES_STRUCT_IFCONF_H