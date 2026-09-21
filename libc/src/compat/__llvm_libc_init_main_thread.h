//===-- Implementation header for __llvm_libc_init_main_thread --*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC_COMPAT___LLVM_LIBC_INIT_MAIN_THREAD_H
#define LLVM_LIBC_SRC_COMPAT___LLVM_LIBC_INIT_MAIN_THREAD_H

#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {

int __llvm_libc_init_main_thread(void);

} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC_COMPAT___LLVM_LIBC_INIT_MAIN_THREAD_H
