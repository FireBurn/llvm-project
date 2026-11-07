//===-- Unittests for the 64 bit signal functions -------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for sigset64_t, struct sigaction64 and the functions that take
/// them.
///
//===----------------------------------------------------------------------===//

#include "hdr/errno_macros.h"
#include "hdr/signal_macros.h"
#include "hdr/types/siginfo_t.h"
#include "hdr/types/sigset64_t.h"
#include "hdr/types/sigset_t.h"
#include "hdr/types/struct_sigaction.h"
#include "hdr/types/struct_sigaction64.h"
#include "hdr/types/struct_timespec.h"
#include "src/signal/kill.h"
#include "src/signal/pthread_sigmask64.h"
#include "src/signal/sigaction64.h"
#include "src/signal/sigaddset64.h"
#include "src/signal/sigdelset64.h"
#include "src/signal/sigemptyset64.h"
#include "src/signal/sigfillset64.h"
#include "src/signal/sigismember64.h"
#include "src/signal/sigpending64.h"
#include "src/signal/sigprocmask64.h"
#include "src/signal/sigtimedwait64.h"
#include "src/signal/sigwait64.h"
#include "src/signal/sigwaitinfo64.h"
#include "src/unistd/getpid.h"
#include "test/UnitTest/ErrnoCheckingTest.h"
#include "test/UnitTest/ErrnoSetterMatcher.h"
#include "test/UnitTest/Test.h"

using LIBC_NAMESPACE::testing::ErrnoSetterMatcher::Fails;
using LIBC_NAMESPACE::testing::ErrnoSetterMatcher::Succeeds;
using LlvmLibcSigset64Test = LIBC_NAMESPACE::testing::ErrnoCheckingTest;

// The two forms are interchangeable on a 64 bit target, which is what
// programs written for bionic assume when they pass one to a function taking
// the other.
static_assert(sizeof(sigset64_t) == sizeof(sigset_t));
static_assert(sizeof(struct sigaction64) == sizeof(struct sigaction));
static_assert(__builtin_offsetof(struct sigaction64, sa_mask) ==
              __builtin_offsetof(struct sigaction, sa_mask));
static_assert(__builtin_offsetof(struct sigaction64, sa_flags) ==
              __builtin_offsetof(struct sigaction, sa_flags));

namespace {
volatile int handled = 0;
void handler(int signal) { handled = signal; }

// A signal is only held for waiting on while it is blocked.
class BlockUsr1 {
public:
  BlockUsr1() {
    LIBC_NAMESPACE::sigemptyset64(&set);
    LIBC_NAMESPACE::sigaddset64(&set, SIGUSR1);
    LIBC_NAMESPACE::sigprocmask64(SIG_BLOCK, &set, &before);
  }
  ~BlockUsr1() { LIBC_NAMESPACE::sigprocmask64(SIG_SETMASK, &before, nullptr); }
  sigset64_t set;
  sigset64_t before;
};
} // namespace

TEST_F(LlvmLibcSigset64Test, SetOperations) {
  sigset64_t set;
  ASSERT_THAT(LIBC_NAMESPACE::sigemptyset64(&set), Succeeds(0));
  EXPECT_EQ(LIBC_NAMESPACE::sigismember64(&set, SIGUSR1), 0);
  ASSERT_THAT(LIBC_NAMESPACE::sigaddset64(&set, SIGUSR1), Succeeds(0));
  EXPECT_EQ(LIBC_NAMESPACE::sigismember64(&set, SIGUSR1), 1);
  ASSERT_THAT(LIBC_NAMESPACE::sigdelset64(&set, SIGUSR1), Succeeds(0));
  EXPECT_EQ(LIBC_NAMESPACE::sigismember64(&set, SIGUSR1), 0);
  ASSERT_THAT(LIBC_NAMESPACE::sigfillset64(&set), Succeeds(0));
  EXPECT_EQ(LIBC_NAMESPACE::sigismember64(&set, SIGUSR2), 1);
  ASSERT_THAT(LIBC_NAMESPACE::sigaddset64(&set, -1), Fails(EINVAL));
}

TEST_F(LlvmLibcSigset64Test, MaskAndPending) {
  BlockUsr1 mask;
  sigset64_t current;
  ASSERT_THAT(LIBC_NAMESPACE::pthread_sigmask64(SIG_BLOCK, nullptr, &current),
              Succeeds(0));
  EXPECT_EQ(LIBC_NAMESPACE::sigismember64(&current, SIGUSR1), 1);

  ASSERT_THAT(LIBC_NAMESPACE::kill(LIBC_NAMESPACE::getpid(), SIGUSR1),
              Succeeds(0));
  sigset64_t pending;
  ASSERT_THAT(LIBC_NAMESPACE::sigpending64(&pending), Succeeds(0));
  EXPECT_EQ(LIBC_NAMESPACE::sigismember64(&pending, SIGUSR1), 1);

  int received = 0;
  ASSERT_EQ(LIBC_NAMESPACE::sigwait64(&mask.set, &received), 0);
  EXPECT_EQ(received, SIGUSR1);
}

TEST_F(LlvmLibcSigset64Test, WaitForASignal) {
  BlockUsr1 mask;
  struct timespec timeout = {0, 1000000};
  siginfo_t info = {};
  ASSERT_THAT(LIBC_NAMESPACE::sigtimedwait64(&mask.set, &info, &timeout),
              Fails(EAGAIN));

  ASSERT_THAT(LIBC_NAMESPACE::kill(LIBC_NAMESPACE::getpid(), SIGUSR1),
              Succeeds(0));
  ASSERT_THAT(LIBC_NAMESPACE::sigwaitinfo64(&mask.set, &info),
              Succeeds(SIGUSR1));
  EXPECT_EQ(info.si_signo, SIGUSR1);
}

TEST_F(LlvmLibcSigset64Test, ActionRunsTheHandler) {
  struct sigaction64 action = {};
  action.sa_handler = handler;
  LIBC_NAMESPACE::sigemptyset64(&action.sa_mask);
  struct sigaction64 old = {};
  ASSERT_THAT(LIBC_NAMESPACE::sigaction64(SIGUSR2, &action, &old), Succeeds(0));
  handled = 0;
  ASSERT_THAT(LIBC_NAMESPACE::kill(LIBC_NAMESPACE::getpid(), SIGUSR2),
              Succeeds(0));
  EXPECT_EQ(handled, SIGUSR2);

  struct sigaction64 current = {};
  ASSERT_THAT(LIBC_NAMESPACE::sigaction64(SIGUSR2, &old, &current),
              Succeeds(0));
  EXPECT_TRUE(current.sa_handler == handler);
}
