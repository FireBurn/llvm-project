//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation of error_one_per_line.
///
//===----------------------------------------------------------------------===//

#include "src/error/error_one_per_line.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {

// Where this is set, error_at_line says nothing about a line it has already
// reported on.
LLVM_LIBC_VARIABLE(int, error_one_per_line) = 0;

} // namespace LIBC_NAMESPACE_DECL
