//===-- Implementation header for free_aligned_sized ------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC_STDLIB_FREE_ALIGNED_SIZED_H
#define LLVM_LIBC_SRC_STDLIB_FREE_ALIGNED_SIZED_H

#include "hdr/types/size_t.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {

void free_aligned_sized(void *ptr, size_t alignment, size_t size);

} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC_STDLIB_FREE_ALIGNED_SIZED_H
