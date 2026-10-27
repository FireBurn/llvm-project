//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Linux implementation of vfork
///
//===----------------------------------------------------------------------===//

#include "src/unistd/vfork.h"
#include "src/__support/OSUtil/linux/syscall_wrappers/gettid.h"
#include "src/__support/OSUtil/linux/syscall_wrappers/vfork.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/__support/threads/identifier.h"

#include "src/__support/libc_errno.h"
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

  ErrorOr<pid_t> ret = linux_syscalls::vfork();

  if (ret && *ret == 0) {
    internal::force_set_tid(linux_syscalls::gettid());
    return 0;
  }

  internal::force_set_tid(parent_tid);

  if (!ret) {
    libc_errno = ret.error();
    return -1;
  }

  return *ret;
}

} // namespace LIBC_NAMESPACE_DECL
