//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Definition of struct arpreq_old.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_TYPES_STRUCT_ARPREQ_OLD_H
#define LLVM_LIBC_TYPES_STRUCT_ARPREQ_OLD_H

#include "struct_sockaddr.h"

#ifdef __ANDROID__
// Android code reads the kernel's definition through <linux/if_arp.h> as well;
// two of them in one translation unit do not compile.
#include <linux/if_arp.h>
#else
/// What struct arpreq was before it said which interface the entry belongs
/// to. It is here because programs older than that change still name it.
struct arpreq_old {
  struct sockaddr arp_pa;
  struct sockaddr arp_ha;
  int arp_flags;
  struct sockaddr arp_netmask;
};
#endif // __ANDROID__

#endif // LLVM_LIBC_TYPES_STRUCT_ARPREQ_OLD_H
