//===-- Implementation header for brk ---------------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC_UNISTD_BRK_H
#define LLVM_LIBC_SRC_UNISTD_BRK_H

#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {

int brk(void *addr);

} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC_UNISTD_BRK_H
