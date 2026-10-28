//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation header for ttyname_r
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC_UNISTD_TTYNAME_R_H
#define LLVM_LIBC_SRC_UNISTD_TTYNAME_R_H

#include "hdr/types/size_t.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {

int ttyname_r(int fd, char *buf, size_t buflen);

namespace internal {
int ttyname_r_impl(int fd, char *buf, size_t buflen);
} // namespace internal

} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC_UNISTD_TTYNAME_R_H
