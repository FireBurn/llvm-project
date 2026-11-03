//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Applies a credential change to every thread of the process.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC___SUPPORT_THREADS_LINUX_SETXID_H
#define LLVM_LIBC_SRC___SUPPORT_THREADS_LINUX_SETXID_H

#include "src/__support/error_or.h"
#include "src/__support/macros/config.h"
#include "src/__support/threads/linux/setxid_signal.h"
#include "src/__support/threads/raw_mutex.h"

namespace LIBC_NAMESPACE_DECL {
namespace internal {

// Held while a thread is created. A thread created during a broadcast would
// copy its credentials from its creator, who may already have changed while
// the new thread was not yet there to be asked.
extern RawMutex thread_creation_lock;

// The change setxid makes, called with the arguments setxid was given. It runs
// in a signal handler, so it is a system call wrapper and nothing more.
using SetxidFn = ErrorOr<int> (*)(long, long, long);

// POSIX has the set*id calls change the credentials of the process, but Linux
// keeps them per thread, so |fn| is called in every thread. The
// calling thread goes first and its result is returned. Should another thread
// then fail where the caller succeeded, the process is left with threads
// running under different credentials, which is not safe to continue with, so
// it aborts, as glibc does.
ErrorOr<int> setxid(SetxidFn fn, long arg0, long arg1 = 0, long arg2 = 0);

} // namespace internal
} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC___SUPPORT_THREADS_LINUX_SETXID_H
