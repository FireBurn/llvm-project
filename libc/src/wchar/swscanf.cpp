//===-- Implementation of swscanf -----------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/wchar/swscanf.h"

#include "hdr/types/wchar_t.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/wchar/vswscanf.h"

#include <stdarg.h>

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(int, swscanf,
                   (const wchar_t *__restrict buffer,
                    const wchar_t *__restrict format, ...)) {
  va_list vlist;
  va_start(vlist, format);
  int ret = LIBC_NAMESPACE::vswscanf(buffer, format, vlist);
  va_end(vlist);
  return ret;
}

} // namespace LIBC_NAMESPACE_DECL
