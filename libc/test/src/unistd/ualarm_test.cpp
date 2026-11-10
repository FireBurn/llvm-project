//===-- Unittests for ualarm ----------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "hdr/signal_macros.h"
#include "hdr/types/struct_timespec.h"
#include "src/signal/signal.h"
#include "src/time/nanosleep.h"
#include "src/unistd/ualarm.h"
#include "test/UnitTest/Test.h"

static volatile int ualarm_fired = 0;
extern "C" void ualarm_handler(int) { ualarm_fired = ualarm_fired + 1; }

TEST(LlvmLibcUalarmTest, ReturnsTheRemainingTime) {
  LIBC_NAMESPACE::signal(SIGALRM, ualarm_handler);
  ualarm_fired = 0;

  // Nothing was pending before.
  EXPECT_EQ(LIBC_NAMESPACE::ualarm(10000000, 0), 0U);
  // The previous ten seconds come back in microseconds, less what has passed.
  useconds_t prev = LIBC_NAMESPACE::ualarm(0, 0);
  EXPECT_GT(prev, 0U);
  EXPECT_LE(prev, 10000000U);
  // Cancelled, so nothing is left and nothing fired.
  EXPECT_EQ(LIBC_NAMESPACE::ualarm(0, 0), 0U);
  EXPECT_EQ(ualarm_fired, 0);
}

TEST(LlvmLibcUalarmTest, FiresOnceWithoutAnInterval) {
  LIBC_NAMESPACE::signal(SIGALRM, ualarm_handler);
  ualarm_fired = 0;
  LIBC_NAMESPACE::ualarm(10000, 0);
  struct timespec tv = {1, 0};
  LIBC_NAMESPACE::nanosleep(&tv, nullptr);
  EXPECT_EQ(ualarm_fired, 1);
}

TEST(LlvmLibcUalarmTest, RepeatsWithAnInterval) {
  LIBC_NAMESPACE::signal(SIGALRM, ualarm_handler);
  ualarm_fired = 0;
  LIBC_NAMESPACE::ualarm(10000, 10000);
  for (int i = 0; i < 50 && ualarm_fired < 3; ++i) {
    struct timespec tv = {0, 20000000};
    LIBC_NAMESPACE::nanosleep(&tv, nullptr);
  }
  LIBC_NAMESPACE::ualarm(0, 0);
  EXPECT_GE(ualarm_fired, 3);
}
