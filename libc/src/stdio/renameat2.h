//===-- Implementation header for renameat2 ---------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC_STDIO_RENAMEAT2_H
#define LLVM_LIBC_SRC_STDIO_RENAMEAT2_H

#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {

int renameat2(int olddirfd, const char *oldpath, int newdirfd,
              const char *newpath, unsigned int flags);

} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC_STDIO_RENAMEAT2_H
