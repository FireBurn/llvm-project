//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Definition of struct sockaddr_in. This is the sockaddr specialization for
/// AF_INET sockets, as defined by posix.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_TYPES_STRUCT_SOCKADDR_IN_H
#define LLVM_LIBC_TYPES_STRUCT_SOCKADDR_IN_H

#include "../llvm-libc-macros/uapi-compat-macros.h"

#include "in_port_t.h"
#include "sa_family_t.h"
#include "struct_in_addr.h"

// The kernel defines this too, in <linux/in.h>. Leaving it to the
// kernel is right only where that header has been read and its
// guard says the definition is the kernel's.
#if !(defined(__UAPI_DEF_SOCKADDR_IN) && __UAPI_DEF_SOCKADDR_IN &&             \
      defined(_LINUX_IN_H))
struct __attribute__((may_alias)) sockaddr_in {
  sa_family_t sin_family; /* AF_INET */
  in_port_t sin_port;
  struct in_addr sin_addr;

  // For historic reasons, AF_INET addresses are 16 bytes. Users are not
  // expected to access the padding field, but it traditionally uses a
  // non-private name.
  char sin_zero[8];
};

// Say the definition here is the one, so <linux/in.h> skips its own if it
// is read after this.
#undef __UAPI_DEF_SOCKADDR_IN
#define __UAPI_DEF_SOCKADDR_IN 0
#endif

#endif // LLVM_LIBC_TYPES_STRUCT_SOCKADDR_IN_H
