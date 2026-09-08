//===-- Definition of __barrier_type type ---------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_TYPES__BARRIER_TYPE_H
#define LLVM_LIBC_TYPES__BARRIER_TYPE_H

// A public header may not include <stdbool.h>: before C23 that defines bool
// as a macro, and every program including <pthread.h> would then have one.
// _Bool is the keyword the macro stood for, and C++ has bool already.
#ifdef __cplusplus
#define __LLVM_LIBC_BOOL bool
#else
#define __LLVM_LIBC_BOOL _Bool
#endif

typedef struct __attribute__((aligned(8 /* alignof (Barrier) */))) {
  unsigned expected;
  unsigned waiting;
  __LLVM_LIBC_BOOL blocking;
  char entering[24 /* sizeof (CndVar) */];
  char exiting[24 /* sizeof (CndVar) */];
  char mutex[24 /* sizeof (Mutex) */];
} __barrier_type;

#undef __LLVM_LIBC_BOOL

#endif // LLVM_LIBC_TYPES__BARRIER_TYPE_H
