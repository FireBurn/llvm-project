//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation header for __llvm_libc_early_init
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC_COMPAT___LLVM_LIBC_EARLY_INIT_H
#define LLVM_LIBC_SRC_COMPAT___LLVM_LIBC_EARLY_INIT_H

#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {

int __llvm_libc_early_init(int argc, char **argv, char **envp,
                           const void *auxv);

} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC_COMPAT___LLVM_LIBC_EARLY_INIT_H
