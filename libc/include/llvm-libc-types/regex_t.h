//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Type definition for regex_t.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_TYPES_REGEX_T_H
#define LLVM_LIBC_TYPES_REGEX_T_H

#include "size_t.h"

// glibc's size, with re_nsub where glibc has it.
typedef struct {
  void *__internal;
  unsigned char __reserved[40];
  size_t re_nsub;
  unsigned char __flags[8];
} regex_t;

#endif // LLVM_LIBC_TYPES_REGEX_T_H
