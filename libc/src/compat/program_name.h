//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Setting the names a program is known by
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC_COMPAT_PROGRAM_NAME_H
#define LLVM_LIBC_SRC_COMPAT_PROGRAM_NAME_H

#include "src/__support/macros/attributes.h"
#include "src/__support/macros/config.h"
#include "src/compat/progname.h"
#include "src/errno/program_invocation_name.h"
#include "src/errno/program_invocation_short_name.h"

namespace LIBC_NAMESPACE_DECL {

// Points the four names at the program's argv[0], and its last component for
// the short ones.
LIBC_INLINE void set_program_name(char *name) {
  program_invocation_name = name;
  program_invocation_short_name = name;
  for (char *p = name; *p != '\0'; ++p)
    if (*p == '/')
      program_invocation_short_name = p + 1;
  // The names BSD gave the same two things.
  __progname_full = program_invocation_name;
  __progname = program_invocation_short_name;
}

} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC_COMPAT_PROGRAM_NAME_H
