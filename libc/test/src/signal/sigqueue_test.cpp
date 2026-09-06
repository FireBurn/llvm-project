//===-- Unittests for the queued signal calls -----------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "hdr/errno_macros.h"
#include "hdr/signal_macros.h"
#include "hdr/types/siginfo_t.h"
#include "hdr/types/sigset_t.h"
#include "hdr/types/struct_timespec.h"
#include "src/signal/killpg.h"
#include "src/signal/sigaddset.h"
#include "src/signal/sigemptyset.h"
#include "src/signal/sigisemptyset.h"
#include "src/signal/sigismember.h"
#include "src/signal/sigpending.h"
#include "src/signal/sigprocmask.h"
#include "src/signal/sigqueue.h"
#include "src/signal/sigtimedwait.h"
#include "src/signal/sigwaitinfo.h"
#include "src/unistd/getpgrp.h"
#include "src/unistd/getpid.h"
#include "test/UnitTest/ErrnoCheckingTest.h"
#include "test/UnitTest/ErrnoSetterMatcher.h"
#include "test/UnitTest/Test.h"

using LIBC_NAMESPACE::testing::ErrnoSetterMatcher::Fails;
using LIBC_NAMESPACE::testing::ErrnoSetterMatcher::Succeeds;

namespace {

// A signal is only queued rather than delivered while it is blocked, so
// every test here blocks the one it uses and puts the mask back after.
class LlvmLibcSigqueueTest : public LIBC_NAMESPACE::testing::ErrnoCheckingTest {
public:
  void SetUp() override {
    LIBC_NAMESPACE::testing::ErrnoCheckingTest::SetUp();
    ASSERT_EQ(LIBC_NAMESPACE::sigemptyset(&blocked), 0);
    ASSERT_EQ(LIBC_NAMESPACE::sigaddset(&blocked, SIGUSR1), 0);
    ASSERT_EQ(LIBC_NAMESPACE::sigprocmask(SIG_BLOCK, &blocked, &before), 0);
  }

  void TearDown() override {
    LIBC_NAMESPACE::sigprocmask(SIG_SETMASK, &before, nullptr);
    LIBC_NAMESPACE::testing::ErrnoCheckingTest::TearDown();
  }

  sigset_t blocked;
  sigset_t before;
};

} // anonymous namespace

TEST_F(LlvmLibcSigqueueTest, EmptySetsAreEmptyAndOthersAreNot) {
  sigset_t set;
  ASSERT_EQ(LIBC_NAMESPACE::sigemptyset(&set), 0);
  ASSERT_EQ(LIBC_NAMESPACE::sigisemptyset(&set), 1);

  ASSERT_EQ(LIBC_NAMESPACE::sigaddset(&set, SIGUSR2), 0);
  ASSERT_EQ(LIBC_NAMESPACE::sigisemptyset(&set), 0);
}

TEST_F(LlvmLibcSigqueueTest, ABlockedSignalIsReportedAsPending) {
  sigset_t pending;
  ASSERT_THAT(LIBC_NAMESPACE::sigpending(&pending), Succeeds(0));
  ASSERT_EQ(LIBC_NAMESPACE::sigismember(&pending, SIGUSR1), 0);

  union sigval value;
  value.sival_int = 42;
  ASSERT_THAT(
      LIBC_NAMESPACE::sigqueue(LIBC_NAMESPACE::getpid(), SIGUSR1, value),
      Succeeds(0));

  ASSERT_THAT(LIBC_NAMESPACE::sigpending(&pending), Succeeds(0));
  ASSERT_EQ(LIBC_NAMESPACE::sigismember(&pending, SIGUSR1), 1);

  // Taking it off the queue carries the value the sender attached.
  siginfo_t info = {};
  ASSERT_THAT(LIBC_NAMESPACE::sigwaitinfo(&blocked, &info), Succeeds(SIGUSR1));
  ASSERT_EQ(info.si_signo, SIGUSR1);
  ASSERT_EQ(info.si_value.sival_int, 42);
}

TEST_F(LlvmLibcSigqueueTest, WaitingWithADeadlineThatPasses) {
  union sigval value;
  value.sival_int = 7;
  ASSERT_THAT(
      LIBC_NAMESPACE::sigqueue(LIBC_NAMESPACE::getpid(), SIGUSR1, value),
      Succeeds(0));

  struct timespec timeout = {};
  siginfo_t info = {};
  ASSERT_THAT(LIBC_NAMESPACE::sigtimedwait(&blocked, &info, &timeout),
              Succeeds(SIGUSR1));
  ASSERT_EQ(info.si_value.sival_int, 7);

  // Nothing is queued now, so the deadline is what ends the wait.
  ASSERT_THAT(LIBC_NAMESPACE::sigtimedwait(&blocked, &info, &timeout),
              Fails(EAGAIN));
}

// killpg signals a whole process group, and zero asks whether it could
// rather than sending anything.
TEST_F(LlvmLibcSigqueueTest, SignallingOurOwnProcessGroup) {
  ASSERT_THAT(LIBC_NAMESPACE::killpg(LIBC_NAMESPACE::getpgrp(), 0),
              Succeeds(0));

  // A group id is never negative, and one that is names nothing.
  ASSERT_THAT(LIBC_NAMESPACE::killpg(-1, 0), Fails(EINVAL));
}
