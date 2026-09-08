//===-- Definition of struct in_addr --------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_TYPES_STRUCT_IN_ADDR_H
#define LLVM_LIBC_TYPES_STRUCT_IN_ADDR_H

#include "../llvm-libc-macros/uapi-compat-macros.h"

#include "in_addr_t.h"

// The kernel defines this too, in <linux/in.h>. Leaving it to the
// kernel is right only where that header has been read and its
// guard says the definition is the kernel's.
#if !(defined(__UAPI_DEF_IN_ADDR) && __UAPI_DEF_IN_ADDR && defined(_LINUX_IN_H))
struct in_addr {
  in_addr_t s_addr;
};

// Say the definition here is the one, so <linux/in.h> skips its own if it
// is read after this.
#undef __UAPI_DEF_IN_ADDR
#define __UAPI_DEF_IN_ADDR 0
#endif

#endif // LLVM_LIBC_TYPES_STRUCT_IN_ADDR_H
