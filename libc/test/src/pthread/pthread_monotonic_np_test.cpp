//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for the waits on the monotonic clock.
///
//===----------------------------------------------------------------------===//

#include "hdr/errno_macros.h"
#include "hdr/time_macros.h"
#include "hdr/types/struct_timespec.h"
#include "src/pthread/pthread_cond_destroy.h"
#include "src/pthread/pthread_cond_init.h"
#include "src/pthread/pthread_cond_timedwait_monotonic_np.h"
#include "src/pthread/pthread_mutex_clocklock.h"
#include "src/pthread/pthread_mutex_destroy.h"
#include "src/pthread/pthread_mutex_init.h"
#include "src/pthread/pthread_mutex_lock.h"
#include "src/pthread/pthread_mutex_timedlock_monotonic_np.h"
#include "src/pthread/pthread_mutex_unlock.h"
#include "src/pthread/pthread_rwlock_destroy.h"
#include "src/pthread/pthread_rwlock_init.h"
#include "src/pthread/pthread_rwlock_timedrdlock_monotonic_np.h"
#include "src/pthread/pthread_rwlock_timedwrlock_monotonic_np.h"
#include "src/pthread/pthread_rwlock_unlock.h"
#include "src/semaphore/sem_destroy.h"
#include "src/semaphore/sem_init.h"
#include "src/semaphore/sem_post.h"
#include "src/semaphore/sem_timedwait_monotonic_np.h"
#include "src/time/clock_gettime.h"
#include "test/UnitTest/ErrnoCheckingTest.h"
#include "test/UnitTest/Test.h"

#include <pthread.h>

using LlvmLibcMonotonicWaitTest = LIBC_NAMESPACE::testing::ErrnoCheckingTest;

namespace {

// A moment on the monotonic clock |milliseconds| from now.
struct timespec monotonic_in(long milliseconds) {
  struct timespec now;
  LIBC_NAMESPACE::clock_gettime(CLOCK_MONOTONIC, &now);
  now.tv_nsec += milliseconds * 1000000L;
  now.tv_sec += now.tv_nsec / 1000000000L;
  now.tv_nsec %= 1000000000L;
  // A moment in the past, early in a second, leaves the nanoseconds negative.
  if (now.tv_nsec < 0) {
    now.tv_nsec += 1000000000L;
    --now.tv_sec;
  }
  return now;
}

} // namespace

TEST_F(LlvmLibcMonotonicWaitTest, MutexTimedLockTakesAFreeMutex) {
  pthread_mutex_t mutex;
  ASSERT_EQ(LIBC_NAMESPACE::pthread_mutex_init(&mutex, nullptr), 0);
  struct timespec deadline = monotonic_in(1000);
  ASSERT_EQ(
      LIBC_NAMESPACE::pthread_mutex_timedlock_monotonic_np(&mutex, &deadline),
      0);
  ASSERT_EQ(LIBC_NAMESPACE::pthread_mutex_unlock(&mutex), 0);
  deadline = monotonic_in(1000);
  ASSERT_EQ(LIBC_NAMESPACE::pthread_mutex_clocklock(&mutex, CLOCK_MONOTONIC,
                                                    &deadline),
            0);
  ASSERT_EQ(LIBC_NAMESPACE::pthread_mutex_unlock(&mutex), 0);
  ASSERT_EQ(LIBC_NAMESPACE::pthread_mutex_destroy(&mutex), 0);
}

TEST_F(LlvmLibcMonotonicWaitTest, MutexTimedLockGivesUpAtTheDeadline) {
  pthread_mutex_t mutex;
  ASSERT_EQ(LIBC_NAMESPACE::pthread_mutex_init(&mutex, nullptr), 0);
  ASSERT_EQ(LIBC_NAMESPACE::pthread_mutex_lock(&mutex), 0);
  struct timespec deadline = monotonic_in(20);
  ASSERT_EQ(
      LIBC_NAMESPACE::pthread_mutex_timedlock_monotonic_np(&mutex, &deadline),
      ETIMEDOUT);
  deadline = monotonic_in(20);
  ASSERT_EQ(LIBC_NAMESPACE::pthread_mutex_clocklock(&mutex, CLOCK_MONOTONIC,
                                                    &deadline),
            ETIMEDOUT);
  // A deadline which has passed still gets the one attempt.
  deadline = monotonic_in(-1);
  ASSERT_EQ(
      LIBC_NAMESPACE::pthread_mutex_timedlock_monotonic_np(&mutex, &deadline),
      ETIMEDOUT);
  ASSERT_EQ(LIBC_NAMESPACE::pthread_mutex_unlock(&mutex), 0);
  ASSERT_EQ(LIBC_NAMESPACE::pthread_mutex_destroy(&mutex), 0);
}

TEST_F(LlvmLibcMonotonicWaitTest, MutexClockLockRefusesWhatItCannotRead) {
  pthread_mutex_t mutex;
  ASSERT_EQ(LIBC_NAMESPACE::pthread_mutex_init(&mutex, nullptr), 0);
  struct timespec deadline = monotonic_in(1000);
  ASSERT_EQ(LIBC_NAMESPACE::pthread_mutex_clocklock(
                &mutex, CLOCK_PROCESS_CPUTIME_ID, &deadline),
            EINVAL);
  ASSERT_EQ(
      LIBC_NAMESPACE::pthread_mutex_clocklock(&mutex, CLOCK_MONOTONIC, nullptr),
      EINVAL);
  deadline.tv_nsec = 1000000000L;
  ASSERT_EQ(LIBC_NAMESPACE::pthread_mutex_clocklock(&mutex, CLOCK_MONOTONIC,
                                                    &deadline),
            EINVAL);
  ASSERT_EQ(LIBC_NAMESPACE::pthread_mutex_destroy(&mutex), 0);
}

TEST_F(LlvmLibcMonotonicWaitTest, RwLockTimedLocksTakeAFreeLock) {
  pthread_rwlock_t rwlock;
  ASSERT_EQ(LIBC_NAMESPACE::pthread_rwlock_init(&rwlock, nullptr), 0);
  struct timespec deadline = monotonic_in(1000);
  ASSERT_EQ(LIBC_NAMESPACE::pthread_rwlock_timedrdlock_monotonic_np(&rwlock,
                                                                    &deadline),
            0);
  ASSERT_EQ(LIBC_NAMESPACE::pthread_rwlock_unlock(&rwlock), 0);
  deadline = monotonic_in(1000);
  ASSERT_EQ(LIBC_NAMESPACE::pthread_rwlock_timedwrlock_monotonic_np(&rwlock,
                                                                    &deadline),
            0);
  ASSERT_EQ(LIBC_NAMESPACE::pthread_rwlock_unlock(&rwlock), 0);
  ASSERT_EQ(LIBC_NAMESPACE::pthread_rwlock_destroy(&rwlock), 0);
}

TEST_F(LlvmLibcMonotonicWaitTest, CondTimedWaitTimesOut) {
  pthread_mutex_t mutex;
  pthread_cond_t cond;
  ASSERT_EQ(LIBC_NAMESPACE::pthread_mutex_init(&mutex, nullptr), 0);
  ASSERT_EQ(LIBC_NAMESPACE::pthread_cond_init(&cond, nullptr), 0);
  ASSERT_EQ(LIBC_NAMESPACE::pthread_mutex_lock(&mutex), 0);
  struct timespec deadline = monotonic_in(20);
  ASSERT_EQ(LIBC_NAMESPACE::pthread_cond_timedwait_monotonic_np(&cond, &mutex,
                                                                &deadline),
            ETIMEDOUT);
  // The mutex is held again on the way out, as it is for every wait.
  ASSERT_EQ(LIBC_NAMESPACE::pthread_mutex_unlock(&mutex), 0);
  ASSERT_EQ(LIBC_NAMESPACE::pthread_cond_destroy(&cond), 0);
  ASSERT_EQ(LIBC_NAMESPACE::pthread_mutex_destroy(&mutex), 0);
}

TEST_F(LlvmLibcMonotonicWaitTest, SemTimedWaitTakesAPostAndTimesOut) {
  sem_t sem;
  ASSERT_EQ(LIBC_NAMESPACE::sem_init(&sem, 0, 1), 0);
  struct timespec deadline = monotonic_in(1000);
  ASSERT_EQ(LIBC_NAMESPACE::sem_timedwait_monotonic_np(&sem, &deadline), 0);
  ASSERT_ERRNO_SUCCESS();
  deadline = monotonic_in(20);
  ASSERT_EQ(LIBC_NAMESPACE::sem_timedwait_monotonic_np(&sem, &deadline), -1);
  ASSERT_ERRNO_EQ(ETIMEDOUT);
  ASSERT_EQ(LIBC_NAMESPACE::sem_destroy(&sem), 0);
}
