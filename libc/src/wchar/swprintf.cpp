//===-- Implementation of swprintf ----------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/wchar/swprintf.h"

#include "hdr/types/size_t.h"
#include "hdr/types/wchar_t.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/wchar/vswprintf.h"

#include <stdarg.h>

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(int, swprintf,
                   (wchar_t *__restrict buffer, size_t bufsz,
                    const wchar_t *__restrict format, ...)) {
  va_list vlist;
  va_start(vlist, format);
  int ret = LIBC_NAMESPACE::vswprintf(buffer, bufsz, format, vlist);
  va_end(vlist);
  return ret;
}

} // namespace LIBC_NAMESPACE_DECL
