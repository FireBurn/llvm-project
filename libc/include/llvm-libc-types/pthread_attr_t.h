//===-- Definition of pthread_attr_t type ---------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_TYPES_PTHREAD_ATTR_T_H
#define LLVM_LIBC_TYPES_PTHREAD_ATTR_T_H

#include "size_t.h"

typedef struct {
  int __detachstate;
  // The scheduling attributes live in what was padding, so the size, which
  // every program already built with one on its stack relies on, is as it
  // was. Zero reads as inherit, SCHED_OTHER and priority 0.
  unsigned char __inheritsched;
  unsigned char __schedpolicy;
  unsigned short __schedpriority;
  void *__stack;
  size_t __stacksize;
  size_t __guardsize;
} pthread_attr_t;

#endif // LLVM_LIBC_TYPES_PTHREAD_ATTR_T_H
