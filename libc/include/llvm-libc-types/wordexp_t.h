//===-- Definition of wordexp_t type --------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_TYPES_WORDEXP_T_H
#define LLVM_LIBC_TYPES_WORDEXP_T_H

#include "size_t.h"

typedef struct {
  size_t we_wordc; // How many words the expansion came to.
  char **we_wordv; // The words themselves, a null pointer after the last.
  size_t we_offs;  // Slots to leave empty at the front of we_wordv.
} wordexp_t;

#endif // LLVM_LIBC_TYPES_WORDEXP_T_H
