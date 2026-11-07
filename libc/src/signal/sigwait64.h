//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation header for sigwait64.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC_SIGNAL_SIGWAIT64_H
#define LLVM_LIBC_SRC_SIGNAL_SIGWAIT64_H

#include "hdr/types/sigset64_t.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {

int sigwait64(const sigset64_t *__restrict set, int *__restrict sig);

} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC_SIGNAL_SIGWAIT64_H
