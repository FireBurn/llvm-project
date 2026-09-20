//===-- Implementation of pthread_mutex_timedlock -------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "pthread_mutex_timedlock.h"

#include "hdr/errno_macros.h"
#include "hdr/types/struct_timespec.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/__support/threads/mutex.h"
#include "src/__support/time/abs_timeout.h"

#include <pthread.h>

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(int, pthread_mutex_timedlock,
                   (pthread_mutex_t *__restrict mutex,
                    const struct timespec *__restrict abstime)) {
  if (abstime == nullptr)
    return EINVAL;

  // The deadline is on the realtime clock, which is what POSIX says this
  // one reads.
  auto deadline =
      internal::AbsTimeout::from_timespec(*abstime, /*is_realtime=*/true);

  MutexError err;
  if (deadline) {
    err = reinterpret_cast<Mutex *>(mutex)->timed_lock(deadline.value());
  } else {
    if (deadline.error() == internal::AbsTimeout::Error::Invalid)
      return EINVAL;
    // A deadline before the epoch has passed already, so the lock is only
    // worth one attempt.
    err = reinterpret_cast<Mutex *>(mutex)->try_lock();
    if (err == MutexError::BUSY)
      return ETIMEDOUT;
  }

  switch (err) {
  case MutexError::NONE:
    return 0;
  case MutexError::TIMEOUT:
    return ETIMEDOUT;
  case MutexError::DEADLOCK:
    return EDEADLK;
  case MutexError::OVERFLOW:
    return EAGAIN;
  default:
    return EINVAL;
  }
}

} // namespace LIBC_NAMESPACE_DECL
