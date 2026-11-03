//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// This file contains the tests of tgkill.
///
//===----------------------------------------------------------------------===//

#include "hdr/errno_macros.h"
#include "hdr/signal_macros.h"
#include "hdr/types/siginfo_t.h"
#include "hdr/types/sigset_t.h"
#include "hdr/types/struct_timespec.h"
#include "src/signal/sigaddset.h"
#include "src/signal/sigemptyset.h"
#include "src/signal/sigismember.h"
#include "src/signal/sigpending.h"
#include "src/signal/sigprocmask.h"
#include "src/signal/sigtimedwait.h"
#include "src/signal/tgkill.h"
#include "src/unistd/getpid.h"
#include "src/unistd/gettid.h"
#include "test/UnitTest/ErrnoCheckingTest.h"
#include "test/UnitTest/ErrnoSetterMatcher.h"
#include "test/UnitTest/Test.h"

using LIBC_NAMESPACE::testing::ErrnoSetterMatcher::Fails;
using LIBC_NAMESPACE::testing::ErrnoSetterMatcher::Succeeds;
using LlvmLibcTgkillTest = LIBC_NAMESPACE::testing::ErrnoCheckingTest;

TEST_F(LlvmLibcTgkillTest, SignalsTheThreadNamed) {
  sigset_t blocked, before;
  ASSERT_EQ(LIBC_NAMESPACE::sigemptyset(&blocked), 0);
  ASSERT_EQ(LIBC_NAMESPACE::sigaddset(&blocked, SIGUSR1), 0);
  ASSERT_EQ(LIBC_NAMESPACE::sigprocmask(SIG_BLOCK, &blocked, &before), 0);

  ASSERT_THAT(LIBC_NAMESPACE::tgkill(LIBC_NAMESPACE::getpid(),
                                     LIBC_NAMESPACE::gettid(), SIGUSR1),
              Succeeds(0));
  sigset_t pending;
  ASSERT_EQ(LIBC_NAMESPACE::sigpending(&pending), 0);
  ASSERT_EQ(LIBC_NAMESPACE::sigismember(&pending, SIGUSR1), 1);

  // Take the signal, so that unblocking does not deliver it.
  siginfo_t info;
  struct timespec none = {0, 0};
  ASSERT_THAT(LIBC_NAMESPACE::sigtimedwait(&blocked, &info, &none),
              Succeeds(SIGUSR1));
  ASSERT_EQ(info.si_code, SI_TKILL);
  LIBC_NAMESPACE::sigprocmask(SIG_SETMASK, &before, nullptr);
}

TEST_F(LlvmLibcTgkillTest, RefusesAnInvalidSignalAndAMismatchedThread) {
  ASSERT_THAT(LIBC_NAMESPACE::tgkill(LIBC_NAMESPACE::getpid(),
                                     LIBC_NAMESPACE::gettid(), -1),
              Fails(EINVAL));
  // A thread which does not belong to the group is not found.
  ASSERT_THAT(LIBC_NAMESPACE::tgkill(1, LIBC_NAMESPACE::gettid(), 0),
              Fails(ESRCH));
}
