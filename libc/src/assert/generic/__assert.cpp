//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation of __assert
///
//===----------------------------------------------------------------------===//

#include "src/assert/__assert.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/assert/__assert_fail.h"

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(void, __assert,
                   (const char *file, int line, const char *message)) {
  LIBC_NAMESPACE::__assert_fail(message, file, static_cast<unsigned>(line), "");
}

} // namespace LIBC_NAMESPACE_DECL
