//===-- How many bytes a character takes ------------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC_STDLIB_MB_CUR_MAX_H
#define LLVM_LIBC_SRC_STDLIB_MB_CUR_MAX_H

#include "hdr/types/size_t.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {

// The most bytes one character takes in the locale in force, which is what
// MB_CUR_MAX reports. Callers size buffers by it before writing a character
// into them, so it has to be no smaller than what wcrtomb will write.
extern size_t __llvm_libc_mb_cur_max;

namespace internal {

void set_mb_cur_max(size_t bytes);

} // namespace internal

} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC_STDLIB_MB_CUR_MAX_H
