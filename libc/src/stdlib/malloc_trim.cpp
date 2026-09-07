//===-- Implementation of malloc_trim -------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/stdlib/malloc_trim.h"

#include "hdr/func/mallopt.h"
#include "include/llvm-libc-macros/malloc-macros.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {

// Hands back to the system whatever the allocator is holding that nothing is
// using. |pad| says how much to keep for what is about to be asked for next;
// the allocator here has no way to be told that, so it is ignored, which
// costs a caller nothing but the memory it was going to ask for anyway.
//
// The answer says whether the allocator did as it was asked, not how much
// came back, since it does not say. A long running program calls this when it
// has just let go of a great deal and would rather the system had it.
LLVM_LIBC_FUNCTION(int, malloc_trim, (size_t pad)) {
  (void)pad;
  return mallopt(M_PURGE, 0);
}

} // namespace LIBC_NAMESPACE_DECL
