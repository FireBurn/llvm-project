//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation header for pthread_sigqueue.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC_SIGNAL_PTHREAD_SIGQUEUE_H
#define LLVM_LIBC_SRC_SIGNAL_PTHREAD_SIGQUEUE_H

#include "hdr/types/pthread_t.h"
#include "hdr/types/union_sigval.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {

int pthread_sigqueue(pthread_t thread, int sig, const union sigval value);

} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC_SIGNAL_PTHREAD_SIGQUEUE_H
