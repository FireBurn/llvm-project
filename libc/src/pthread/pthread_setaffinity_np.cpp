//===-- Implementation of pthread_setaffinity_np -------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/pthread/pthread_setaffinity_np.h"

#include "hdr/errno_macros.h"
#include "src/__support/OSUtil/syscall.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/__support/threads/thread.h"

#include <sys/syscall.h>

namespace LIBC_NAMESPACE_DECL {

// Sets which processors a thread may run on. The kernel knows a thread by its
// id, so that is what the pthread_t is turned into. Unlike the sched calls
// these report the error rather than setting errno, as the rest of the
// pthread interface does.
LLVM_LIBC_FUNCTION(int, pthread_setaffinity_np,
                   (pthread_t th, size_t cpuset_size, const cpu_set_t *mask)) {
  auto *thread = reinterpret_cast<Thread *>(&th);
  if (thread->attrib == nullptr)
    return ESRCH;
  const int ret = LIBC_NAMESPACE::syscall_impl<int>(
      SYS_sched_setaffinity, thread->attrib->tid, cpuset_size, mask);
  if (ret < 0)
    return -ret;
  return 0;
}

} // namespace LIBC_NAMESPACE_DECL
