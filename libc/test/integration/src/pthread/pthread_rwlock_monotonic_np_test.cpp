//===-- Integration tests for the rwlock waits on the monotonic clock -----===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Integration tests for pthread_rwlock_timedrdlock_monotonic_np and
/// pthread_rwlock_timedwrlock_monotonic_np. A thread has to be the one that
/// waits: the thread which holds a write lock is refused at once.
///
//===----------------------------------------------------------------------===//

#include "hdr/errno_macros.h"
#include "hdr/time_macros.h"
#include "hdr/types/struct_timespec.h"
#include "src/pthread/pthread_create.h"
#include "src/pthread/pthread_join.h"
#include "src/pthread/pthread_rwlock_init.h"
#include "src/pthread/pthread_rwlock_timedrdlock_monotonic_np.h"
#include "src/pthread/pthread_rwlock_timedwrlock_monotonic_np.h"
#include "src/pthread/pthread_rwlock_unlock.h"
#include "src/pthread/pthread_rwlock_wrlock.h"
#include "src/time/clock_gettime.h"
#include "test/IntegrationTest/test.h"

#include <pthread.h>

static struct timespec monotonic_in(long nanoseconds) {
  struct timespec now;
  LIBC_NAMESPACE::clock_gettime(CLOCK_MONOTONIC, &now);
  now.tv_nsec += nanoseconds;
  while (now.tv_nsec >= 1000000000L) {
    now.tv_nsec -= 1000000000L;
    ++now.tv_sec;
  }
  return now;
}

static int results[2];

static void *waits(void *arg) {
  auto *rwlock = static_cast<pthread_rwlock_t *>(arg);
  struct timespec deadline = monotonic_in(20000000L);
  results[0] = LIBC_NAMESPACE::pthread_rwlock_timedrdlock_monotonic_np(
      rwlock, &deadline);
  deadline = monotonic_in(20000000L);
  results[1] = LIBC_NAMESPACE::pthread_rwlock_timedwrlock_monotonic_np(
      rwlock, &deadline);
  return nullptr;
}

TEST_MAIN() {
  pthread_rwlock_t rwlock;
  ASSERT_EQ(LIBC_NAMESPACE::pthread_rwlock_init(&rwlock, nullptr), 0);
  ASSERT_EQ(LIBC_NAMESPACE::pthread_rwlock_wrlock(&rwlock), 0);

  pthread_t thread;
  ASSERT_EQ(LIBC_NAMESPACE::pthread_create(&thread, nullptr, waits, &rwlock),
            0);
  void *value;
  ASSERT_EQ(LIBC_NAMESPACE::pthread_join(thread, &value), 0);
  ASSERT_EQ(results[0], ETIMEDOUT);
  ASSERT_EQ(results[1], ETIMEDOUT);

  ASSERT_EQ(LIBC_NAMESPACE::pthread_rwlock_unlock(&rwlock), 0);
  return 0;
}
