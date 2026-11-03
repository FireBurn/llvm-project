//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation of pthread_sigqueue.
///
//===----------------------------------------------------------------------===//

#include "src/signal/pthread_sigqueue.h"
#include "hdr/errno_macros.h"
#include "hdr/signal_macros.h"
#include "hdr/types/pthread_t.h"
#include "hdr/types/siginfo_t.h"
#include "hdr/types/union_sigval.h"
#include "src/__support/OSUtil/linux/syscall_wrappers/getpid.h"
#include "src/__support/OSUtil/linux/syscall_wrappers/getuid.h"
#include "src/__support/OSUtil/linux/syscall_wrappers/rt_tgsigqueueinfo.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/__support/macros/null_check.h"
#include "src/__support/threads/thread.h"

namespace LIBC_NAMESPACE_DECL {

// As sigqueue does, but to one thread of this process, so the value is
// delivered inside a siginfo_t that says it was queued.
LLVM_LIBC_FUNCTION(int, pthread_sigqueue,
                   (pthread_t th, int sig, const union sigval value)) {
  auto *thread = reinterpret_cast<Thread *>(&th);
  LIBC_CRASH_ON_NULLPTR(thread->attrib);

  if (sig < 0 || sig >= NSIG)
    return EINVAL;

  siginfo_t info;
  __builtin_memset(&info, 0, sizeof(info));
  info.si_signo = sig;
  info.si_code = SI_QUEUE;
  info.si_pid = linux_syscalls::getpid();
  info.si_uid = static_cast<unsigned int>(linux_syscalls::getuid());
  info.si_value = value;

  auto result = linux_syscalls::rt_tgsigqueueinfo(
      linux_syscalls::getpid(), thread->attrib->tid, sig, &info);
  return result.has_value() ? 0 : result.error();
}

} // namespace LIBC_NAMESPACE_DECL
