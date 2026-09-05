//===-- Unittests for sigwait ---------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "hdr/errno_macros.h"
#include "hdr/signal_macros.h"
#include "src/__support/libc_errno.h"
#include "src/signal/raise.h"
#include "src/signal/sigaddset.h"
#include "src/signal/sigemptyset.h"
#include "src/signal/sigprocmask.h"
#include "src/signal/sigwait.h"
#include "test/UnitTest/ErrnoCheckingTest.h"
#include "test/UnitTest/Test.h"

using LlvmLibcSigwaitTest = LIBC_NAMESPACE::testing::ErrnoCheckingTest;

TEST_F(LlvmLibcSigwaitTest, ReportsWhichSignalArrived) {
  sigset_t set;
  LIBC_NAMESPACE::sigemptyset(&set);
  LIBC_NAMESPACE::sigaddset(&set, SIGUSR1);

  sigset_t previous;
  ASSERT_EQ(LIBC_NAMESPACE::sigprocmask(SIG_BLOCK, &set, &previous), 0);

  // Raised while blocked, so it waits for this rather than for anything to
  // happen later.
  ASSERT_EQ(LIBC_NAMESPACE::raise(SIGUSR1), 0);

  int signal = 0;
  EXPECT_EQ(LIBC_NAMESPACE::sigwait(&set, &signal), 0);
  EXPECT_EQ(signal, SIGUSR1);

  ASSERT_EQ(LIBC_NAMESPACE::sigprocmask(SIG_SETMASK, &previous, nullptr), 0);
}

TEST_F(LlvmLibcSigwaitTest, LeavesErrnoAlone) {
  sigset_t set;
  LIBC_NAMESPACE::sigemptyset(&set);
  LIBC_NAMESPACE::sigaddset(&set, SIGUSR2);

  sigset_t previous;
  ASSERT_EQ(LIBC_NAMESPACE::sigprocmask(SIG_BLOCK, &set, &previous), 0);
  ASSERT_EQ(LIBC_NAMESPACE::raise(SIGUSR2), 0);

  // What went wrong comes back as the result, so whatever errno held is
  // still there afterwards.
  libc_errno = EDOM;
  int signal = 0;
  EXPECT_EQ(LIBC_NAMESPACE::sigwait(&set, &signal), 0);
  EXPECT_EQ(signal, SIGUSR2);
  EXPECT_EQ(static_cast<int>(libc_errno), EDOM);
  libc_errno = 0;

  ASSERT_EQ(LIBC_NAMESPACE::sigprocmask(SIG_SETMASK, &previous, nullptr), 0);
}

TEST_F(LlvmLibcSigwaitTest, TakesANullSignalPointer) {
  sigset_t set;
  LIBC_NAMESPACE::sigemptyset(&set);
  LIBC_NAMESPACE::sigaddset(&set, SIGUSR1);

  sigset_t previous;
  ASSERT_EQ(LIBC_NAMESPACE::sigprocmask(SIG_BLOCK, &set, &previous), 0);
  ASSERT_EQ(LIBC_NAMESPACE::raise(SIGUSR1), 0);
  EXPECT_EQ(LIBC_NAMESPACE::sigwait(&set, nullptr), 0);
  ASSERT_EQ(LIBC_NAMESPACE::sigprocmask(SIG_SETMASK, &previous, nullptr), 0);
}
