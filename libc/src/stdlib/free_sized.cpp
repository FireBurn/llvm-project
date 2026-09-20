//===-- Implementation of free_sized --------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/stdlib/free_sized.h"
#include "hdr/func/free.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {

// The size is a hint that free may use and nothing more. The memory goes to
// free, as a program which puts an allocator of its own in place of malloc
// and free expects: glib hands everything it allocated to free_sized, and
// chromium replaces free but has no free_sized of its own.
LLVM_LIBC_FUNCTION(void, free_sized,
                   (void *ptr, [[maybe_unused]] size_t size)) {
  ::free(ptr);
}

} // namespace LIBC_NAMESPACE_DECL
