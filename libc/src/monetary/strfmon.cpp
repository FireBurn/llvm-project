//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation of strfmon
///
//===----------------------------------------------------------------------===//

#include "src/monetary/strfmon.h"

#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/monetary/strfmon_impl.h"

#include <stdarg.h>

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(ssize_t, strfmon,
                   (char *__restrict s, size_t maxsize,
                    const char *__restrict format, ...)) {
  va_list args;
  va_start(args, format);
  ssize_t result = monetary::strfmon(s, maxsize, format, args);
  va_end(args);
  return result;
}

} // namespace LIBC_NAMESPACE_DECL
