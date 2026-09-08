//===-- Definition of pthread_barrierattr_t type --------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_TYPES_PTHREAD_BARRIERATTR_T_H
#define LLVM_LIBC_TYPES_PTHREAD_BARRIERATTR_T_H

// A public header may not include <stdbool.h>: before C23 that defines bool
// as a macro, and every program including <pthread.h> would then have one.
// _Bool is the keyword the macro stood for, and C++ has bool already.
#ifdef __cplusplus
#define __LLVM_LIBC_BOOL bool
#else
#define __LLVM_LIBC_BOOL _Bool
#endif

typedef struct {
  __LLVM_LIBC_BOOL pshared;
} pthread_barrierattr_t;

#undef __LLVM_LIBC_BOOL

#endif // LLVM_LIBC_TYPES_PTHREAD_BARRIERATTR_T_H
