//===-- Implementation of fprintf -------------------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/stdio/fprintf.h"
#include "src/stdio/vfprintf.h"

#include "src/__support/common.h"
#include "src/__support/macros/config.h"

#include "hdr/types/FILE.h"
#include <stdarg.h>

namespace LIBC_NAMESPACE_DECL {

namespace internal {

int fprintf_impl(::FILE *__restrict stream, const char *__restrict format,
                 ...) {
  va_list vlist;
  va_start(vlist, format);
  int ret = internal::vfprintf_impl(stream, format, vlist);
  va_end(vlist);
  return ret;
}

} // namespace internal

LLVM_LIBC_FUNCTION(int, fprintf,
                   (::FILE *__restrict stream, const char *__restrict format,
                    ...)) {
  va_list vlist;
  va_start(vlist, format);
  int ret = internal::vfprintf_impl(stream, format, vlist);
  va_end(vlist);
  return ret;
}

} // namespace LIBC_NAMESPACE_DECL
