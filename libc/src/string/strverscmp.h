//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation header for strverscmp
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC_STRING_STRVERSCMP_H
#define LLVM_LIBC_SRC_STRING_STRVERSCMP_H

#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {

int strverscmp(const char *lhs, const char *rhs);

namespace internal {
int strverscmp_impl(const char *lhs, const char *rhs);
} // namespace internal

} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC_STRING_STRVERSCMP_H
