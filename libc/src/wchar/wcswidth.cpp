//===-- Implementation of wcswidth ----------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/wchar/wcswidth.h"

#include "hdr/types/size_t.h"
#include "hdr/types/wchar_t.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/wchar/wcwidth.h"

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(int, wcswidth, (const wchar_t *ws, size_t n)) {
  if (ws == nullptr)
    return -1;
  int total = 0;
  for (size_t i = 0; i < n && ws[i] != L'\0'; ++i) {
    int width = LIBC_NAMESPACE::wcwidth(ws[i]);
    // A single character which cannot be displayed makes the whole string
    // unmeasurable.
    if (width < 0)
      return -1;
    total += width;
  }
  return total;
}

} // namespace LIBC_NAMESPACE_DECL
