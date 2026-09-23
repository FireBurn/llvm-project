//===-- Definition of pthread_cond_t type ---------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_TYPES_PTHREAD_COND_T_H
#define LLVM_LIBC_TYPES_PTHREAD_COND_T_H

#include "__futex_word.h"
#include "size_t.h"

typedef struct {
  union {
    void *__waiter_queue[2];
    size_t __waiter_size;
  };
  __futex_word __futex;
  char __is_shared;
  // Zero for CLOCK_REALTIME, so that a condition variable that is all zeros
  // is a valid one, as it is with glibc.
  char __is_monotonic;
  char __padding[2];
  // Unused, and there to give the type glibc's size.
  char __reserved[24];
} pthread_cond_t;

#endif // LLVM_LIBC_TYPES_PTHREAD_COND_T_H
