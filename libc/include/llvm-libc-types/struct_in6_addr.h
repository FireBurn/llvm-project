//===-- Definition of struct in6_addr -------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_TYPES_STRUCT_IN6_ADDR_H
#define LLVM_LIBC_TYPES_STRUCT_IN6_ADDR_H

#include "../llvm-libc-macros/uapi-compat-macros.h"

#include "../llvm-libc-macros/stdint-macros.h"

// The kernel defines this too, in <linux/in6.h>. Leaving it to the
// kernel is right only where that header has been read and its
// guard says the definition is the kernel's.
#if !(defined(__UAPI_DEF_IN6_ADDR) && __UAPI_DEF_IN6_ADDR &&                   \
      defined(_LINUX_IN6_H))
// The three ways of looking at the same sixteen octets. They are reached
// through macros rather than named directly, which is how every other library
// and the kernel's own header spell them: code that has to work where one of
// the wider two is missing tests for it with #ifndef, and a plain member
// cannot be tested for that way.
struct in6_addr {
  union {
    uint8_t __u6_addr8[16];
    uint16_t __u6_addr16[8];
    uint32_t __u6_addr32[4];
  } __in6_u;
};

#define s6_addr __in6_u.__u6_addr8
#define s6_addr16 __in6_u.__u6_addr16
#define s6_addr32 __in6_u.__u6_addr32

// Say the definition here is the one, so <linux/in6.h> skips its own if it
// is read after this.
#undef __UAPI_DEF_IN6_ADDR
#define __UAPI_DEF_IN6_ADDR 0
#endif

#endif // LLVM_LIBC_TYPES_STRUCT_IN6_ADDR_H
