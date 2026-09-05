//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for sigtimedwait and sigwaitinfo
///
//===----------------------------------------------------------------------===//

#include "hdr/errno_macros.h"
#include "hdr/signal_macros.h"
#include "hdr/types/siginfo_t.h"
#include "hdr/types/sigset_t.h"
#include "hdr/types/struct_timespec.h"
#include "src/signal/kill.h"
#include "src/signal/sigaddset.h"
#include "src/signal/sigemptyset.h"
#include "src/signal/sigprocmask.h"
#include "src/signal/sigtimedwait.h"
#include "src/signal/sigwaitinfo.h"
#include "src/unistd/getpid.h"
#include "test/UnitTest/ErrnoCheckingTest.h"
#include "test/UnitTest/ErrnoSetterMatcher.h"
#include "test/UnitTest/Test.h"

using LIBC_NAMESPACE::testing::ErrnoSetterMatcher::Fails;
using LIBC_NAMESPACE::testing::ErrnoSetterMatcher::Succeeds;
using LlvmLibcSigtimedwaitTest = LIBC_NAMESPACE::testing::ErrnoCheckingTest;

// A signal is only held for waiting on while it is blocked.
class SigtimedwaitMask {
public:
  SigtimedwaitMask() {
    LIBC_NAMESPACE::sigemptyset(&set);
    LIBC_NAMESPACE::sigaddset(&set, SIGUSR1);
    LIBC_NAMESPACE::sigprocmask(SIG_BLOCK, &set, &before);
  }
  ~SigtimedwaitMask() {
    LIBC_NAMESPACE::sigprocmask(SIG_SETMASK, &before, nullptr);
  }
  sigset_t set;
  sigset_t before;
};

TEST_F(LlvmLibcSigtimedwaitTest, TakesABlockedSignalOffTheQueue) {
  SigtimedwaitMask mask;
  ASSERT_THAT(LIBC_NAMESPACE::kill(LIBC_NAMESPACE::getpid(), SIGUSR1),
              Succeeds(0));
  siginfo_t info = {};
  struct timespec timeout = {1, 0};
  ASSERT_THAT(LIBC_NAMESPACE::sigtimedwait(&mask.set, &info, &timeout),
              Succeeds(SIGUSR1));
  ASSERT_EQ(info.si_signo, SIGUSR1);
}

TEST_F(LlvmLibcSigtimedwaitTest, TheDeadlineEndsTheWait) {
  SigtimedwaitMask mask;
  siginfo_t info = {};
  struct timespec timeout = {0, 1000000};
  ASSERT_THAT(LIBC_NAMESPACE::sigtimedwait(&mask.set, &info, &timeout),
              Fails(EAGAIN));
}

TEST_F(LlvmLibcSigtimedwaitTest, RejectsABadTimeout) {
  SigtimedwaitMask mask;
  struct timespec bad = {0, 1000000000L};
  ASSERT_THAT(LIBC_NAMESPACE::sigtimedwait(&mask.set, nullptr, &bad),
              Fails(EINVAL));
}

TEST_F(LlvmLibcSigtimedwaitTest, SigwaitinfoWaitsWithoutADeadline) {
  SigtimedwaitMask mask;
  ASSERT_THAT(LIBC_NAMESPACE::kill(LIBC_NAMESPACE::getpid(), SIGUSR1),
              Succeeds(0));
  siginfo_t info = {};
  ASSERT_THAT(LIBC_NAMESPACE::sigwaitinfo(&mask.set, &info),
              Succeeds(SIGUSR1));
  ASSERT_EQ(info.si_signo, SIGUSR1);
}
