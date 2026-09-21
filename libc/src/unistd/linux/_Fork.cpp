//===-- Linux implementation of _Fork -------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/unistd/_Fork.h"

#include "hdr/signal_macros.h"            // For SIGCHLD
#include "src/__support/OSUtil/syscall.h" // For internal syscall function.
#include "src/__support/common.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"
#include "src/__support/threads/identifier.h"

#include <sys/syscall.h> // For syscall numbers.

namespace LIBC_NAMESPACE_DECL {

// fork without the pthread_atfork handlers. POSIX added this so a process
// can fork from a signal handler: running the handlers there is what makes
// fork itself unsafe to call from one.
LLVM_LIBC_FUNCTION(pid_t, _Fork, (void)) {
  pid_t parent_tid = internal::gettid();
  // Invalidate the parent's cached tid before forking, for the same reason
  // fork does: a signal handler running in the window after the syscall
  // would otherwise read the wrong one.
  internal::force_set_tid(0);
#ifdef SYS_fork
  pid_t ret = syscall_impl<pid_t>(SYS_fork);
#elif defined(SYS_clone)
  pid_t ret = syscall_impl<pid_t>(SYS_clone, SIGCHLD, 0);
#else
#error "fork and clone syscalls not available."
#endif

  if (ret == 0) {
    internal::force_set_tid(syscall_impl<pid_t>(SYS_gettid));
    return 0;
  }

  internal::force_set_tid(parent_tid);
  if (ret < 0) {
    libc_errno = static_cast<int>(-ret);
    return -1;
  }
  return ret;
}

} // namespace LIBC_NAMESPACE_DECL
