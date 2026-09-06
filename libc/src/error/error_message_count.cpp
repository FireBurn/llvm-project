//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation of error_message_count.
///
//===----------------------------------------------------------------------===//

#include "src/error/error_message_count.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {

// Counts the messages error and error_at_line have written.
LLVM_LIBC_VARIABLE(unsigned int, error_message_count) = 0;

} // namespace LIBC_NAMESPACE_DECL
