//===-- Implementation header for pthread_kill ------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC_PTHREAD_PTHREAD_KILL_H
#define LLVM_LIBC_SRC_PTHREAD_PTHREAD_KILL_H

#include <pthread.h>

#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {

// Raises a signal for one thread of this process rather than for the process
// as a whole. Answers with an error number rather than setting errno, as the
// thread calls do.
int pthread_kill(pthread_t th, int sig);

} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC_PTHREAD_PTHREAD_KILL_H
