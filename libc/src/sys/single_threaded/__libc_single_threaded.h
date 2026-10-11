//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation header for __libc_single_threaded
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC_SYS_SINGLE_THREADED___LIBC_SINGLE_THREADED_H
#define LLVM_LIBC_SRC_SYS_SINGLE_THREADED___LIBC_SINGLE_THREADED_H

#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {

extern LIBC_SHARED_INTERNAL char __libc_single_threaded;

} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC_SYS_SINGLE_THREADED___LIBC_SINGLE_THREADED_H
