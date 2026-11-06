//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Waiting for a mutex until a deadline.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC_PTHREAD_PTHREAD_MUTEX_TIMED_LOCK_UTILS_H
#define LLVM_LIBC_SRC_PTHREAD_PTHREAD_MUTEX_TIMED_LOCK_UTILS_H

#include "hdr/errno_macros.h"
#include "hdr/types/struct_timespec.h"
#include "src/__support/macros/attributes.h"
#include "src/__support/macros/config.h"
#include "src/__support/threads/mutex.h"
#include "src/__support/time/abs_timeout.h"

#include <pthread.h>

namespace LIBC_NAMESPACE_DECL {
namespace pthread_mutex_utils {

// What pthread_mutex_timedlock and the functions like it answer, for a
// deadline read on the realtime clock or on the monotonic one.
LIBC_INLINE int timed_lock(pthread_mutex_t *mutex,
                           const struct timespec *abstime, bool is_realtime) {
  if (abstime == nullptr)
    return EINVAL;

  auto deadline = internal::AbsTimeout::from_timespec(*abstime, is_realtime);
  Mutex *m = reinterpret_cast<Mutex *>(mutex);

  MutexError err;
  if (deadline) {
    err = m->timed_lock(deadline.value());
  } else {
    if (deadline.error() == internal::AbsTimeout::Error::Invalid)
      return EINVAL;
    // A deadline before the epoch has passed already, so the lock is only
    // worth one attempt.
    err = m->try_lock();
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
  case MutexError::OWNER_DEAD:
    return EOWNERDEAD;
  case MutexError::NOT_RECOVERABLE:
    return ENOTRECOVERABLE;
  default:
    return EINVAL;
  }
}

} // namespace pthread_mutex_utils
} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC_PTHREAD_PTHREAD_MUTEX_TIMED_LOCK_UTILS_H
