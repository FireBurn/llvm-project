//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Integration tests for pthread_getcpuclockid.
///
//===----------------------------------------------------------------------===//

#include "hdr/types/clockid_t.h"
#include "hdr/types/struct_timespec.h"
#include "src/pthread/pthread_create.h"
#include "src/pthread/pthread_getcpuclockid.h"
#include "src/pthread/pthread_join.h"
#include "src/pthread/pthread_self.h"
#include "src/time/clock_gettime.h"
#include "test/IntegrationTest/test.h"

#include <pthread.h>

static long long nanoseconds(const struct timespec &t) {
  return static_cast<long long>(t.tv_sec) * 1000000000LL + t.tv_nsec;
}

static void reads_the_clock_of_the_calling_thread() {
  clockid_t clock;
  ASSERT_EQ(LIBC_NAMESPACE::pthread_getcpuclockid(
                LIBC_NAMESPACE::pthread_self(), &clock),
            0);
  struct timespec first;
  ASSERT_EQ(LIBC_NAMESPACE::clock_gettime(clock, &first), 0);
  // Spend some time of our own so the clock has to move.
  volatile unsigned long sink = 0;
  for (unsigned long i = 0; i < 20000000UL; ++i)
    sink = sink + i;
  struct timespec second;
  ASSERT_EQ(LIBC_NAMESPACE::clock_gettime(clock, &second), 0);
  ASSERT_TRUE(nanoseconds(second) > nanoseconds(first));
}

static void *report_own_clock(void *out) {
  *static_cast<clockid_t *>(out) = 0;
  clockid_t clock;
  if (LIBC_NAMESPACE::pthread_getcpuclockid(LIBC_NAMESPACE::pthread_self(),
                                            &clock) != 0)
    return reinterpret_cast<void *>(1);
  *static_cast<clockid_t *>(out) = clock;
  return nullptr;
}

// Each thread has a clock of its own.
static void threads_have_different_clocks() {
  clockid_t main_clock, thread_clock;
  ASSERT_EQ(LIBC_NAMESPACE::pthread_getcpuclockid(
                LIBC_NAMESPACE::pthread_self(), &main_clock),
            0);
  pthread_t tid;
  ASSERT_EQ(LIBC_NAMESPACE::pthread_create(&tid, nullptr, report_own_clock,
                                           &thread_clock),
            0);
  void *result = reinterpret_cast<void *>(2);
  ASSERT_EQ(LIBC_NAMESPACE::pthread_join(tid, &result), 0);
  ASSERT_TRUE(result == nullptr);
  ASSERT_TRUE(thread_clock != main_clock);
}

TEST_MAIN() {
  reads_the_clock_of_the_calling_thread();
  threads_have_different_clocks();
  return 0;
}
