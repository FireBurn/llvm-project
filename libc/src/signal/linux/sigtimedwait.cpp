//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Linux implementation of sigtimedwait
///
//===----------------------------------------------------------------------===//

#include "src/signal/sigtimedwait.h"

#include "hdr/errno_macros.h"
#include "hdr/types/siginfo_t.h"
#include "hdr/types/sigset_t.h"
#include "hdr/types/struct_timespec.h"
#include "src/__support/OSUtil/linux/syscall_wrappers/rt_sigtimedwait.h"
#include "src/__support/common.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"
#include "src/__support/threads/cancel.h"
namespace LIBC_NAMESPACE_DECL {

namespace internal {

int sigtimedwait_impl(const sigset_t *__restrict set,
                      siginfo_t *__restrict info,
                      const struct timespec *__restrict timeout) {
  internal::cancel_point();
  // The kernel wants to be told how wide the set is, since its own is
  // narrower than the one the library hands out.
  ErrorOr<int> ret = linux_syscalls::rt_sigtimedwait(set, info, timeout);
  if (!ret && ret.error() == EINTR)
    internal::cancel_point();

  if (!ret) {
    libc_errno = ret.error();
    return -1;
  }
  return *ret;
}

} // namespace internal

LLVM_LIBC_FUNCTION(int, sigtimedwait,
                   (const sigset_t *__restrict set, siginfo_t *__restrict info,
                    const struct timespec *__restrict timeout)) {
  return internal::sigtimedwait_impl(set, info, timeout);
}

} // namespace LIBC_NAMESPACE_DECL
