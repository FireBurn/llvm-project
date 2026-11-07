//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation header for sigtimedwait64.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC_SIGNAL_SIGTIMEDWAIT64_H
#define LLVM_LIBC_SRC_SIGNAL_SIGTIMEDWAIT64_H

#include "hdr/types/siginfo_t.h"
#include "hdr/types/sigset64_t.h"
#include "hdr/types/struct_timespec.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {

int sigtimedwait64(const sigset64_t *__restrict set, siginfo_t *__restrict info,
                   const struct timespec *__restrict timeout);

} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC_SIGNAL_SIGTIMEDWAIT64_H
