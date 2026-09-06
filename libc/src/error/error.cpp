//===-- Implementation of error -------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/error/error.h"
#include "src/__support/OSUtil/exit.h"
#include "src/__support/arg_list.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/error/report.h"

#include <stdarg.h>

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(void, error,
                   (int status, int errnum, const char *format, ...)) {
  va_list args;
  va_start(args, format);
  internal::ArgList arg_list(args);
  error_reporting::report(errnum, nullptr, 0, /*with_location=*/false, format,
                          arg_list);
  va_end(args);
  if (status != 0)
    internal::exit(status);
}

} // namespace LIBC_NAMESPACE_DECL
