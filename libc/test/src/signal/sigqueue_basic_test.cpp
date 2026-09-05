//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for sigqueue and sigisemptyset
///
//===----------------------------------------------------------------------===//

#include "hdr/errno_macros.h"
#include "hdr/signal_macros.h"
#include "hdr/types/siginfo_t.h"
#include "hdr/types/sigset_t.h"
#include "src/signal/sigaddset.h"
#include "src/signal/sigemptyset.h"
#include "src/signal/sigisemptyset.h"
#include "src/signal/sigprocmask.h"
#include "src/signal/sigqueue.h"
#include "src/signal/sigwaitinfo.h"
#include "src/unistd/getpid.h"
#include "test/UnitTest/ErrnoCheckingTest.h"
#include "test/UnitTest/ErrnoSetterMatcher.h"
#include "test/UnitTest/Test.h"

using LIBC_NAMESPACE::testing::ErrnoSetterMatcher::Fails;
using LIBC_NAMESPACE::testing::ErrnoSetterMatcher::Succeeds;
using LlvmLibcSigqueueBasicTest = LIBC_NAMESPACE::testing::ErrnoCheckingTest;

TEST_F(LlvmLibcSigqueueBasicTest, EmptySetsAreEmptyAndOthersAreNot) {
  sigset_t set;
  ASSERT_EQ(LIBC_NAMESPACE::sigemptyset(&set), 0);
  ASSERT_EQ(LIBC_NAMESPACE::sigisemptyset(&set), 1);
  ASSERT_EQ(LIBC_NAMESPACE::sigaddset(&set, SIGUSR2), 0);
  ASSERT_EQ(LIBC_NAMESPACE::sigisemptyset(&set), 0);
}

TEST_F(LlvmLibcSigqueueBasicTest, TheValueSentIsTheValueReceived) {
  sigset_t blocked, before;
  ASSERT_EQ(LIBC_NAMESPACE::sigemptyset(&blocked), 0);
  ASSERT_EQ(LIBC_NAMESPACE::sigaddset(&blocked, SIGUSR1), 0);
  ASSERT_EQ(LIBC_NAMESPACE::sigprocmask(SIG_BLOCK, &blocked, &before), 0);

  union sigval value;
  value.sival_int = 42;
  ASSERT_THAT(
      LIBC_NAMESPACE::sigqueue(LIBC_NAMESPACE::getpid(), SIGUSR1, value),
      Succeeds(0));
  siginfo_t info = {};
  ASSERT_THAT(LIBC_NAMESPACE::sigwaitinfo(&blocked, &info), Succeeds(SIGUSR1));
  ASSERT_EQ(info.si_code, SI_QUEUE);
  ASSERT_EQ(info.si_value.sival_int, 42);

  LIBC_NAMESPACE::sigprocmask(SIG_SETMASK, &before, nullptr);
}

TEST_F(LlvmLibcSigqueueBasicTest, AnInvalidSignalIsRefused) {
  union sigval value;
  value.sival_int = 0;
  ASSERT_THAT(LIBC_NAMESPACE::sigqueue(LIBC_NAMESPACE::getpid(), -1, value),
              Fails(EINVAL));
}
