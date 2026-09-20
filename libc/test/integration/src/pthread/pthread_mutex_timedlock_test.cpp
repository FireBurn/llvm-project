//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Integration tests for pthread_mutex_timedlock.
///
//===----------------------------------------------------------------------===//

#include "hdr/errno_macros.h"
#include "hdr/time_macros.h"
#include "hdr/types/struct_timespec.h"
#include "src/pthread/pthread_mutex_init.h"
#include "src/pthread/pthread_mutex_lock.h"
#include "src/pthread/pthread_mutex_timedlock.h"
#include "src/pthread/pthread_mutex_unlock.h"
#include "src/time/clock_gettime.h"
#include "test/IntegrationTest/test.h"

#include <pthread.h>

static struct timespec after(long nanoseconds) {
  struct timespec now;
  LIBC_NAMESPACE::clock_gettime(CLOCK_REALTIME, &now);
  now.tv_nsec += nanoseconds;
  while (now.tv_nsec >= 1000000000L) {
    now.tv_nsec -= 1000000000L;
    ++now.tv_sec;
  }
  return now;
}

static void takes_a_free_mutex() {
  pthread_mutex_t mutex;
  ASSERT_EQ(LIBC_NAMESPACE::pthread_mutex_init(&mutex, nullptr), 0);
  struct timespec deadline = after(1000000000L);
  ASSERT_EQ(LIBC_NAMESPACE::pthread_mutex_timedlock(&mutex, &deadline), 0);
  ASSERT_EQ(LIBC_NAMESPACE::pthread_mutex_unlock(&mutex), 0);
}

static void gives_up_on_a_held_mutex() {
  pthread_mutex_t mutex;
  ASSERT_EQ(LIBC_NAMESPACE::pthread_mutex_init(&mutex, nullptr), 0);
  ASSERT_EQ(LIBC_NAMESPACE::pthread_mutex_lock(&mutex), 0);

  struct timespec deadline = after(20000000L);
  ASSERT_EQ(LIBC_NAMESPACE::pthread_mutex_timedlock(&mutex, &deadline),
            ETIMEDOUT);
  // The wait lasted until the deadline and no longer than a little past it.
  struct timespec now;
  LIBC_NAMESPACE::clock_gettime(CLOCK_REALTIME, &now);
  ASSERT_TRUE(now.tv_sec > deadline.tv_sec ||
              (now.tv_sec == deadline.tv_sec && now.tv_nsec >= deadline.tv_nsec));

  // A deadline already past fails without waiting.
  struct timespec past = {1, 0};
  ASSERT_EQ(LIBC_NAMESPACE::pthread_mutex_timedlock(&mutex, &past), ETIMEDOUT);
  ASSERT_EQ(LIBC_NAMESPACE::pthread_mutex_unlock(&mutex), 0);
}

static void refuses_a_malformed_time() {
  pthread_mutex_t mutex;
  ASSERT_EQ(LIBC_NAMESPACE::pthread_mutex_init(&mutex, nullptr), 0);
  struct timespec bad = {0, 1000000000L};
  ASSERT_EQ(LIBC_NAMESPACE::pthread_mutex_timedlock(&mutex, &bad), EINVAL);
  struct timespec negative = {0, -1};
  ASSERT_EQ(LIBC_NAMESPACE::pthread_mutex_timedlock(&mutex, &negative), EINVAL);
}

TEST_MAIN() {
  takes_a_free_mutex();
  gives_up_on_a_held_mutex();
  refuses_a_malformed_time();
  return 0;
}
