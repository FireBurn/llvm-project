//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation of setprogname.
///
//===----------------------------------------------------------------------===//

#include "src/stdlib/setprogname.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/__support/macros/null_check.h"
#include "src/compat/progname.h"

namespace LIBC_NAMESPACE_DECL {

// Keeps the pointer rather than a copy, as the BSDs do, so the string has to
// outlive the program's use of the name.
LLVM_LIBC_FUNCTION(void, setprogname, (const char *name)) {
  LIBC_CRASH_ON_NULLPTR(name);
  char *base = const_cast<char *>(name);
  for (char *p = base; *p != '\0'; ++p)
    if (*p == '/')
      base = p + 1;
  __progname = base;
}

} // namespace LIBC_NAMESPACE_DECL
