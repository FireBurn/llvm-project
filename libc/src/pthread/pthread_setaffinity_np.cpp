//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation of pthread_setaffinity_np
///
//===----------------------------------------------------------------------===//

#include "src/pthread/pthread_setaffinity_np.h"

#include "hdr/errno_macros.h"
#include "src/__support/OSUtil/linux/syscall_wrappers/sched_setaffinity.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/__support/threads/thread.h"

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
  const ErrorOr<int> ret = linux_syscalls::sched_setaffinity(
      thread->attrib->tid,
      cpp::span<const unsigned char>(
          reinterpret_cast<const unsigned char *>(mask), cpuset_size));
  if (!ret)
    return ret.error();
  return 0;
}

} // namespace LIBC_NAMESPACE_DECL
