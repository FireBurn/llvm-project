//===-- Implementation of pthread_kill ------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/pthread/pthread_kill.h"

#include "hdr/errno_macros.h"
#include "hdr/signal_macros.h"
#include "src/__support/OSUtil/syscall.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/__support/threads/thread.h"

#include <sys/syscall.h>

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(int, pthread_kill, (pthread_t th, int sig)) {
  if (sig < 0 || sig >= NSIG)
    return EINVAL;

  auto *thread = reinterpret_cast<Thread *>(&th);
  if (thread->attrib == nullptr)
    return ESRCH;

  // The signal goes to the one thread, which is what sets this apart from
  // kill. The process is named as well as the thread so that a thread
  // identifier reused by another process cannot be signalled by mistake.
  const long result = LIBC_NAMESPACE::syscall_impl<long>(
      SYS_tgkill, LIBC_NAMESPACE::syscall_impl<long>(SYS_getpid),
      thread->attrib->tid, sig);
  if (result < 0)
    return static_cast<int>(-result);
  return 0;
}

} // namespace LIBC_NAMESPACE_DECL
