//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation of __assert_perror_fail
///
//===----------------------------------------------------------------------===//

#include "src/assert/__assert_perror_fail.h"
#include "src/__support/OSUtil/io.h"
#include "src/__support/StringUtil/error_to_string.h"
#include "src/__support/common.h"
#include "src/__support/integer_to_string.h"
#include "src/__support/macros/config.h"
#include "src/stdlib/abort_utils.h"

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(void, __assert_perror_fail,
                   (int errnum, const char *file, unsigned line,
                    const char *function)) {
  const IntegerToString<unsigned> line_buffer(line);
  write_to_stderr(file);
  write_to_stderr(":");
  write_to_stderr(line_buffer.view());
  write_to_stderr(": Unexpected error: ");
  write_to_stderr(get_error_string(errnum));
  write_to_stderr(" in function: '");
  write_to_stderr(function);
  write_to_stderr("'\n");
  abort_utils::abort();
}

} // namespace LIBC_NAMESPACE_DECL
