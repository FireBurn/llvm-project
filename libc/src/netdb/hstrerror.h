//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation header for hstrerror
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC_NETDB_HSTRERROR_H
#define LLVM_LIBC_SRC_NETDB_HSTRERROR_H

#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {

const char *hstrerror(int err_num);

namespace internal {
const char *hstrerror_impl(int err_num);
} // namespace internal

} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC_NETDB_HSTRERROR_H
