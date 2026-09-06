//===-- Definition of fpos64_t type ---------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_TYPES_FPOS64_T_H
#define LLVM_LIBC_TYPES_FPOS64_T_H

#include "fpos_t.h"

// A file position is already the wide one, so the name large file support
// gave it stands for the same type.
typedef fpos_t fpos64_t;

#endif // LLVM_LIBC_TYPES_FPOS64_T_H
