//===-- Definition of type glob_t -----------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_TYPES_GLOB_T_H
#define LLVM_LIBC_TYPES_GLOB_T_H

#include "size_t.h"

typedef struct {
  size_t gl_pathc; // The number of paths matched.
  char **gl_pathv; // The matched paths, followed by a null pointer.
  size_t gl_offs;  // Slots to leave empty at the front of gl_pathv.
} glob_t;

#endif // LLVM_LIBC_TYPES_GLOB_T_H
