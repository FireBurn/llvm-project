//===-- Definition of a common mutex type ---------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_TYPES___MUTEX_TYPE_H
#define LLVM_LIBC_TYPES___MUTEX_TYPE_H

#include "__futex_word.h"
#include "pid_t.h"
#include "size_t.h"

// glibc's layout, which a program built for glibc initialises statically.
// __kind holds the type in its low two bits and the robust, priority
// inheriting and process shared flags above them, as glibc's does.
typedef struct {
#ifdef __linux__
  __futex_word __ftxw;
#else
#error "Mutex type not defined for the target platform."
#endif
  unsigned int __count;
  pid_t __owner;
  unsigned int __nusers;
  int __kind;
  int __spins;
  // Where a robust mutex is on its owner's list.
  void *__reserved[2];
} __mutex_type;

#endif // LLVM_LIBC_TYPES___MUTEX_TYPE_H
