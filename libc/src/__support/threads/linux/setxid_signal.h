//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// The signal setxid asks the other threads to change their credentials with.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC___SUPPORT_THREADS_LINUX_SETXID_SIGNAL_H
#define LLVM_LIBC_SRC___SUPPORT_THREADS_LINUX_SETXID_SIGNAL_H

#include "hdr/signal_macros.h"
#include "hdr/types/sigset_t.h"
#include "src/__support/OSUtil/linux/sigmask.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {
namespace internal {

// Like CANCEL_SIGNAL it belongs to the library: sigprocmask and
// pthread_sigmask leave it unblocked, so that every thread can be reached, and
// SIGRTMIN is above it. glibc uses the same number for the same purpose.
constexpr int SETXID_SIGNAL = 33;

// The mask sigprocmask or pthread_sigmask is to install: |set| with
// SETXID_SIGNAL taken out when it would otherwise be blocked.
LIBC_INLINE const sigset_t *keep_setxid_unblocked(int how, const sigset_t *set,
                                                  sigset_t &copy) {
  if (set == nullptr || how == SIG_UNBLOCK)
    return set;
  copy = *set;
  signals::remove(copy, SETXID_SIGNAL);
  return &copy;
}

} // namespace internal
} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC___SUPPORT_THREADS_LINUX_SETXID_SIGNAL_H
