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

#include "../llvm-libc-macros/uapi-compat-macros.h"

#include "struct_ifreq.h"

// The kernel defines this too, in <linux/if.h>. Leaving it to the
// kernel is right only where that header has been read and its
// guard says the definition is the kernel's.
#if !(__UAPI_DEF_IF_IFCONF && defined(_LINUX_IF_H))
// What the SIOCGIFCONF ioctl fills in. On the way in ifc_len is the room in
// the block; on the way out it is how much of it was used.
struct ifconf {
  int ifc_len;
  __extension__ union {
    char *ifc_buf;
    struct ifreq *ifc_req;
  };
};
#endif

#endif // LLVM_LIBC_TYPES_STRUCT_IFCONF_H
