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
#include "src/__support/OSUtil/linux/syscall_wrappers/rt_sigprocmask.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {
namespace internal {

// Like CANCEL_SIGNAL it belongs to the library: sigprocmask and
// pthread_sigmask leave it unblocked, so that every thread can be reached, and
// SIGRTMIN is above it. glibc uses the same number for the same purpose.
constexpr int SETXID_SIGNAL = 33;

// Called by sigprocmask and pthread_sigmask once the mask |set| asked for is
// in place, so that the kernel has already checked |set| can be read. A mask
// which blocked SETXID_SIGNAL has it unblocked again.
LIBC_INLINE void keep_setxid_unblocked(int how, const sigset_t *set) {
  if (set == nullptr || how == SIG_UNBLOCK)
    return;
  auto blocked = signals::is_member(*set, SETXID_SIGNAL);
  if (blocked.has_value() && blocked.value()) {
    sigset_t only = signals::empty_set();
    signals::add(only, SETXID_SIGNAL);
    (void)linux_syscalls::rt_sigprocmask(SIG_UNBLOCK, &only, nullptr);
  }
}

} // namespace internal
} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC___SUPPORT_THREADS_LINUX_SETXID_SIGNAL_H
