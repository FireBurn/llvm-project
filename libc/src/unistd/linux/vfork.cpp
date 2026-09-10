//===-- Linux implementation of vfork -------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/unistd/vfork.h"
#include "hdr/sched_macros.h"             // For CLONE_VFORK
#include "hdr/signal_macros.h"            // For SIGCHLD
#include "src/__support/OSUtil/syscall.h" // For internal syscall function.
#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/__support/threads/identifier.h"

#include "src/__support/libc_errno.h"
#include <sys/syscall.h> // For syscall numbers.

namespace LIBC_NAMESPACE_DECL {

// What callers rely on vfork for is that the parent stays suspended until the
// child execs or exits, and CLONE_VFORK alone gives that. The address space is
// deliberately not shared: sharing it is what forces other libcs to write
// vfork in assembly, since a child returning from a C vfork would walk over
// the frame its suspended parent resumes into. A program that keeps to what
// vfork permits between the call and the exec cannot tell the difference.
//
// The atfork handlers registered by pthread_atfork are not run. vfork is not
// a fork for these purposes: the child is not expected to return to the
// program, so there is nothing in it for a handler to fix up.
LLVM_LIBC_FUNCTION(pid_t, vfork, (void)) {
  pid_t parent_tid = internal::gettid();
  // Invalidate parent's tid cache before cloning. We cannot do this in the
  // child because in the post-clone instruction window, there may be a signal
  // handler triggered which may get the wrong tid.
  internal::force_set_tid(0);

  pid_t ret = syscall_impl<pid_t>(SYS_clone, CLONE_VFORK | SIGCHLD, 0);

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
