//===-- Implementation of free_aligned_sized ------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/stdlib/free_aligned_sized.h"
#include "hdr/func/free.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {

// As free_sized: the memory goes to free, which may be the program's own.
LLVM_LIBC_FUNCTION(void, free_aligned_sized,
                   (void *ptr, [[maybe_unused]] size_t alignment,
                    [[maybe_unused]] size_t size)) {
  ::free(ptr);
}

} // namespace LIBC_NAMESPACE_DECL
