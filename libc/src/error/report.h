//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation header for the message error and error_at_line write.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC_ERROR_REPORT_H
#define LLVM_LIBC_SRC_ERROR_REPORT_H

#include "src/__support/arg_list.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {
namespace error_reporting {

// Writes one message. A file name, where there is one, is followed by the
// line number; where there is none the two are left out and only the space
// that would have followed them is written, which is what the GNU
// implementation does.
void report(int err_num, const char *file_name, unsigned int line_number,
            bool with_location, const char *fmt, internal::ArgList &args);

} // namespace error_reporting
} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC_ERROR_REPORT_H
