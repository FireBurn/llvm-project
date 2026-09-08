//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Definition of struct group_req.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_TYPES_STRUCT_GROUP_REQ_H
#define LLVM_LIBC_TYPES_STRUCT_GROUP_REQ_H

#include "../llvm-libc-macros/uapi-compat-macros.h"

#include "../llvm-libc-macros/stdint-macros.h"
#include "struct_sockaddr_storage.h"

// The kernel defines this too, in <linux/in.h>. Leaving it to the
// kernel is right only where that header has been read and its
// guard says the definition is the kernel's.
#if !(defined(__UAPI_DEF_IP_MREQ) && __UAPI_DEF_IP_MREQ && defined(_LINUX_IN_H))
struct group_req {
  uint32_t gr_interface;
  // NB: Architecture-specific padding.
  struct sockaddr_storage gr_group;
};

// Say the definition here is the one, so <linux/in.h> skips its own if it
// is read after this.
#undef __UAPI_DEF_IP_MREQ
#define __UAPI_DEF_IP_MREQ 0
#endif

#endif // LLVM_LIBC_TYPES_STRUCT_GROUP_REQ_H
