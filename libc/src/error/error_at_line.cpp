//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation of error_at_line
///
//===----------------------------------------------------------------------===//

#include "src/error/error_at_line.h"
#include "src/__support/OSUtil/exit.h"
#include "src/__support/arg_list.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/error/error_one_per_line.h"
#include "src/error/report.h"
#include "src/string/memory_utils/inline_strcmp.h"

#include <stdarg.h>

namespace LIBC_NAMESPACE_DECL {

namespace {

// The last place reported on, kept so that a caller which has asked for one
// message per line can be given one.
const char *last_file_name = nullptr;
unsigned int last_line_number = 0;

LIBC_INLINE bool same_place(const char *file_name, unsigned int line_number) {
  if (line_number != last_line_number)
    return false;
  if (file_name == last_file_name)
    return true;
  if (file_name == nullptr || last_file_name == nullptr)
    return false;
  auto comp = [](char l, char r) -> int {
    return static_cast<unsigned char>(l) - static_cast<unsigned char>(r);
  };
  return inline_strcmp(file_name, last_file_name, comp) == 0;
}

} // anonymous namespace

LLVM_LIBC_FUNCTION(void, error_at_line,
                   (int status, int errnum, const char *filename,
                    unsigned int linenum, const char *format, ...)) {
  if (error_one_per_line != 0) {
    if (same_place(filename, linenum))
      return;
    last_file_name = filename;
    last_line_number = linenum;
  }

  va_list args;
  va_start(args, format);
  internal::ArgList arg_list(args);
  error_reporting::report(errnum, filename, linenum, /*with_location=*/true,
                          format, arg_list);
  va_end(args);
  if (status != 0)
    internal::exit(status);
}

} // namespace LIBC_NAMESPACE_DECL
