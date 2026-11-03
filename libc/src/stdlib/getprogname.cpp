//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation of getprogname.
///
//===----------------------------------------------------------------------===//

#include "src/stdlib/getprogname.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/compat/progname.h"

namespace LIBC_NAMESPACE_DECL {

// The name the program was started as, without its directory. Startup code
// sets it, so a program with no startup code has none.
LLVM_LIBC_FUNCTION(const char *, getprogname, (void)) {
  return __progname != nullptr ? __progname : "";
}

} // namespace LIBC_NAMESPACE_DECL
