//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Definition of struct ifmap.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_TYPES_STRUCT_IFMAP_H
#define LLVM_LIBC_TYPES_STRUCT_IFMAP_H

#include "../llvm-libc-macros/uapi-compat-macros.h"

// The kernel defines this too, in <linux/if.h>. Leaving it to the
// kernel is right only where that header has been read and its
// guard says the definition is the kernel's.
#if !(defined(__UAPI_DEF_IF_IFMAP) && __UAPI_DEF_IF_IFMAP &&                   \
      defined(_LINUX_IF_H))
struct ifmap {
  unsigned long mem_start;
  unsigned long mem_end;
  unsigned short base_addr;
  unsigned char irq;
  unsigned char dma;
  unsigned char port;
};

// Say the definition here is the one, so <linux/if.h> skips its own if it
// is read after this.
#undef __UAPI_DEF_IF_IFMAP
#define __UAPI_DEF_IF_IFMAP 0
#endif

#endif // LLVM_LIBC_TYPES_STRUCT_IFMAP_H
