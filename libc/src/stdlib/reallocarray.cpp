//===-- Implementation of reallocarray ------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/stdlib/reallocarray.h"

#include "hdr/errno_macros.h"
#include "hdr/func/realloc.h"
#include "hdr/types/size_t.h"
#include "src/__support/common.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(void *, reallocarray,
                   (void *ptr, size_t nmemb, size_t size)) {
  size_t bytes;
  // The whole point of reallocarray over realloc is that the multiplication
  // is checked rather than silently wrapping into a short allocation.
  if (__builtin_mul_overflow(nmemb, size, &bytes)) {
    libc_errno = ENOMEM;
    return nullptr;
  }
  return realloc(ptr, bytes);
}

} // namespace LIBC_NAMESPACE_DECL
