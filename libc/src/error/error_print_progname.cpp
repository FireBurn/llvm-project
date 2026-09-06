//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation of error_print_progname.
///
//===----------------------------------------------------------------------===//

#include "src/error/error_print_progname.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {

// The macro below names the type and the variable in that order, which a
// function pointer type cannot be spelled in.
using ErrorPrintProgname = void (*)(void);

// Where this is set, it writes the prefix of the message in place of the
// program name and the colon that would otherwise start it.
LLVM_LIBC_VARIABLE(ErrorPrintProgname, error_print_progname) = nullptr;

} // namespace LIBC_NAMESPACE_DECL
