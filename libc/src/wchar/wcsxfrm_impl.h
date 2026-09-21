//===-- Shared transform behind wcsxfrm and wcsxfrm_l -----------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC_WCHAR_WCSXFRM_IMPL_H
#define LLVM_LIBC_SRC_WCHAR_WCSXFRM_IMPL_H

#include "hdr/types/size_t.h"
#include "hdr/types/wchar_t.h"
#include "src/__support/macros/attributes.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {
namespace internal {

// TODO: Add support for locale-aware collation keys.
// For now, this implements C/POSIX-like behavior: the transformed form is the
// original wide string itself, so comparing transformed strings with wcscmp
// matches code-point order.
LIBC_INLINE size_t wcsxfrm_impl(wchar_t *__restrict dest,
                                const wchar_t *__restrict src, size_t n) {
  // Number of source characters that may be written before the trailing NUL.
  const size_t write_limit = n > 0 ? n - 1 : 0;

  size_t i = 0;

  // Single pass over the prefix we might need to copy.
  // This avoids a full wcslen(src) pass for the common case where the source
  // fits in the destination buffer.
  for (; i < write_limit; ++i) {
    const wchar_t ch = src[i];
    if (ch == L'\0') {
      dest[i] = L'\0';
      return i;
    }
    dest[i] = ch;
  }

  // If n > 0, always NUL-terminate. This is correct both when truncating and
  // when write_limit == 0 (i.e. n == 1).
  if (n > 0)
    dest[write_limit] = L'\0';

  // Finish counting the remaining source length if we truncated or if n == 0.
  while (src[i] != L'\0')
    ++i;

  return i;
}

} // namespace internal
} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC_WCHAR_WCSXFRM_IMPL_H
