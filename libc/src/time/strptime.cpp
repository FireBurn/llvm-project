//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// This file contains the definition of the strptime function.
///
//===----------------------------------------------------------------------===//

#include "src/time/strptime.h"
#include "hdr/types/struct_tm.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/time/strptime_core/parser.h"

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(char *, strptime,
                   (const char *__restrict buf, const char *__restrict format,
                    struct tm *__restrict tm)) {
  strptime_core::ParseState state;
  const char *end = strptime_core::parse(buf, format, tm, state);
  if (end == nullptr)
    return nullptr;
  strptime_core::complete(tm, state);
  return const_cast<char *>(end);
}

} // namespace LIBC_NAMESPACE_DECL
