//===-- The BSD names for the program's own name --------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/compat/progname.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {

// What BSD called the two names a program is known by, and what a great deal
// of code still refers to. They hold what program_invocation_short_name and
// program_invocation_name do, and the startup code sets all four together.
LLVM_LIBC_VARIABLE(char *, __progname) = nullptr;
LLVM_LIBC_VARIABLE(char *, __progname_full) = nullptr;

} // namespace LIBC_NAMESPACE_DECL
