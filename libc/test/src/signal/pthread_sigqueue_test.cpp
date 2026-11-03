//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// This file contains the tests of pthread_sigqueue.
///
//===----------------------------------------------------------------------===//

#include "hdr/errno_macros.h"
#include "hdr/signal_macros.h"
#include "hdr/types/siginfo_t.h"
#include "hdr/types/sigset_t.h"
#include "hdr/types/struct_timespec.h"
#include "hdr/types/union_sigval.h"
#include "src/pthread/pthread_self.h"
#include "src/signal/pthread_sigqueue.h"
#include "src/signal/sigaddset.h"
#include "src/signal/sigemptyset.h"
#include "src/signal/sigprocmask.h"
#include "src/signal/sigtimedwait.h"
#include "src/unistd/getpid.h"
#include "test/UnitTest/ErrnoCheckingTest.h"
#include "test/UnitTest/ErrnoSetterMatcher.h"
#include "test/UnitTest/Test.h"

using LIBC_NAMESPACE::testing::ErrnoSetterMatcher::Succeeds;
using LlvmLibcPthreadSigqueueTest = LIBC_NAMESPACE::testing::ErrnoCheckingTest;

TEST_F(LlvmLibcPthreadSigqueueTest, DeliversTheValueToTheThread) {
  sigset_t blocked, before;
  ASSERT_EQ(LIBC_NAMESPACE::sigemptyset(&blocked), 0);
  ASSERT_EQ(LIBC_NAMESPACE::sigaddset(&blocked, SIGUSR1), 0);
  ASSERT_EQ(LIBC_NAMESPACE::sigprocmask(SIG_BLOCK, &blocked, &before), 0);

  union sigval value;
  value.sival_int = 4242;
  ASSERT_EQ(LIBC_NAMESPACE::pthread_sigqueue(LIBC_NAMESPACE::pthread_self(),
                                             SIGUSR1, value),
            0);

  siginfo_t info;
  struct timespec none = {0, 0};
  ASSERT_THAT(LIBC_NAMESPACE::sigtimedwait(&blocked, &info, &none),
              Succeeds(SIGUSR1));
  ASSERT_EQ(info.si_code, SI_QUEUE);
  ASSERT_EQ(info.si_value.sival_int, 4242);
  ASSERT_EQ(info.si_pid, LIBC_NAMESPACE::getpid());
  LIBC_NAMESPACE::sigprocmask(SIG_SETMASK, &before, nullptr);
}

TEST_F(LlvmLibcPthreadSigqueueTest, RefusesAnInvalidSignal) {
  union sigval value;
  value.sival_int = 0;
  ASSERT_EQ(LIBC_NAMESPACE::pthread_sigqueue(LIBC_NAMESPACE::pthread_self(), -1,
                                             value),
            EINVAL);
}
