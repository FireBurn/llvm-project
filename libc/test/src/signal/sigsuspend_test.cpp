//===-- Unittests for sigsuspend ------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "hdr/signal_macros.h"
#include "hdr/types/sigset_t.h"
#include "src/signal/sigaction.h"
#include "src/signal/sigaddset.h"
#include "src/signal/sigemptyset.h"
#include "src/signal/sigsuspend.h"
#include "src/unistd/alarm.h"
#include "test/UnitTest/ErrnoCheckingTest.h"
#include "test/UnitTest/ErrnoSetterMatcher.h"
#include "test/UnitTest/Test.h"

using namespace LIBC_NAMESPACE::testing::ErrnoSetterMatcher;
using LlvmLibcSigSuspendTest = LIBC_NAMESPACE::testing::ErrnoCheckingTest;

namespace {

volatile int handler_runs = 0;

void count_handler(int) { ++handler_runs; }

void catch_alarm() {
  struct sigaction action;
  action.sa_handler = count_handler;
  action.sa_flags = 0;
  LIBC_NAMESPACE::sigemptyset(&action.sa_mask);
  LIBC_NAMESPACE::sigaction(SIGALRM, &action, nullptr);
}

} // anonymous namespace

TEST_F(LlvmLibcSigSuspendTest, WaitsForASignal) {
  catch_alarm();
  handler_runs = 0;

  sigset_t mask;
  ASSERT_THAT(LIBC_NAMESPACE::sigemptyset(&mask), Succeeds());

  // The alarm is the only thing that ends the wait, and sigsuspend always
  // reports a failure when it returns.
  ASSERT_EQ(LIBC_NAMESPACE::alarm(1), 0u);
  EXPECT_THAT(LIBC_NAMESPACE::sigsuspend(&mask), Fails(EINTR, -1));
  EXPECT_EQ(handler_runs, 1);
}

TEST_F(LlvmLibcSigSuspendTest, NullMaskFails) {
  EXPECT_THAT(LIBC_NAMESPACE::sigsuspend(nullptr), Fails(EINVAL, -1));
}

TEST_F(LlvmLibcSigSuspendTest, BadPointerFails) {
  EXPECT_THAT(LIBC_NAMESPACE::sigsuspend(reinterpret_cast<sigset_t *>(-1)),
              Fails(EFAULT, -1));
}
