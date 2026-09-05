//===-- Linux implementation of sigtimedwait ------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/signal/sigtimedwait.h"

#include "hdr/types/siginfo_t.h"
#include "hdr/types/sigset_t.h"
#include "hdr/types/struct_timespec.h"
#include "src/__support/OSUtil/syscall.h" // For internal syscall function.
#include "src/__support/common.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"
#include <sys/syscall.h> // For syscall numbers.

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(int, sigtimedwait,
                   (const sigset_t *__restrict set, siginfo_t *__restrict info,
                    const struct timespec *__restrict timeout)) {
  // The kernel wants to be told how wide the set is, since its own is
  // narrower than the one the library hands out.
  int ret = LIBC_NAMESPACE::syscall_impl<int>(SYS_rt_sigtimedwait, set, info,
                                              timeout, sizeof(sigset_t));
  if (ret < 0) {
    libc_errno = -ret;
    return -1;
  }
  return ret;
}

} // namespace LIBC_NAMESPACE_DECL
