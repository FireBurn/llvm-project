//===-- Definition of pthread_condattr_t type -----------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
#ifndef LLVM_LIBC_TYPES_PTHREAD_CONDATTR_T_H
#define LLVM_LIBC_TYPES_PTHREAD_CONDATTR_T_H

#include "clockid_t.h"

// The four bytes glibc gives the type. Bit 0 is pshared, and the clock is in
// the bits above it, as in glibc. Use the accessors in
// src/pthread/pthread_condattr_bits.h.
typedef struct {
  unsigned int __value;
} pthread_condattr_t;

#endif // LLVM_LIBC_TYPES_PTHREAD_CONDATTR_T_H
