//===-- Implementation of wprintf -----------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/wchar/wprintf.h"

#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/wchar/vwprintf.h"

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(int, wprintf, (const wchar_t *__restrict format, ...)) {
  va_list vlist;
  va_start(vlist, format);
  int ret = LIBC_NAMESPACE::vwprintf(format, vlist);
  va_end(vlist);
  return ret;
}

} // namespace LIBC_NAMESPACE_DECL
