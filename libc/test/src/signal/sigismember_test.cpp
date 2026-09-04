//===-- Unittests for sigismember -----------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/signal/sigaddset.h"
#include "src/signal/sigdelset.h"
#include "src/signal/sigemptyset.h"
#include "src/signal/sigfillset.h"
#include "src/signal/sigismember.h"

#include "hdr/signal_macros.h"
#include "hdr/types/sigset_t.h"
#include "test/UnitTest/ErrnoCheckingTest.h"
#include "test/UnitTest/ErrnoSetterMatcher.h"
#include "test/UnitTest/Test.h"

using namespace LIBC_NAMESPACE::testing::ErrnoSetterMatcher;
using LlvmLibcSigIsMemberTest = LIBC_NAMESPACE::testing::ErrnoCheckingTest;

TEST_F(LlvmLibcSigIsMemberTest, EmptySetHoldsNothing) {
  sigset_t set;
  ASSERT_THAT(LIBC_NAMESPACE::sigemptyset(&set), Succeeds());
  EXPECT_THAT(LIBC_NAMESPACE::sigismember(&set, SIGINT), Succeeds(0));
  EXPECT_THAT(LIBC_NAMESPACE::sigismember(&set, SIGTERM), Succeeds(0));
}

TEST_F(LlvmLibcSigIsMemberTest, FullSetHoldsEverything) {
  sigset_t set;
  ASSERT_THAT(LIBC_NAMESPACE::sigfillset(&set), Succeeds());
  EXPECT_THAT(LIBC_NAMESPACE::sigismember(&set, SIGINT), Succeeds(1));
  EXPECT_THAT(LIBC_NAMESPACE::sigismember(&set, SIGTERM), Succeeds(1));
}

TEST_F(LlvmLibcSigIsMemberTest, TracksAddAndDelete) {
  sigset_t set;
  ASSERT_THAT(LIBC_NAMESPACE::sigemptyset(&set), Succeeds());
  ASSERT_THAT(LIBC_NAMESPACE::sigaddset(&set, SIGUSR1), Succeeds());

  EXPECT_THAT(LIBC_NAMESPACE::sigismember(&set, SIGUSR1), Succeeds(1));
  // Only the signal which was added is a member.
  EXPECT_THAT(LIBC_NAMESPACE::sigismember(&set, SIGUSR2), Succeeds(0));

  ASSERT_THAT(LIBC_NAMESPACE::sigdelset(&set, SIGUSR1), Succeeds());
  EXPECT_THAT(LIBC_NAMESPACE::sigismember(&set, SIGUSR1), Succeeds(0));
}

TEST_F(LlvmLibcSigIsMemberTest, Invalid) {
  sigset_t set;
  ASSERT_THAT(LIBC_NAMESPACE::sigemptyset(&set), Succeeds());

  constexpr int BITS_IN_SIGSET_T = 8 * sizeof(sigset_t);
  EXPECT_THAT(LIBC_NAMESPACE::sigismember(nullptr, SIGINT), Fails(EINVAL, -1));
  EXPECT_THAT(LIBC_NAMESPACE::sigismember(&set, -1), Fails(EINVAL, -1));
  EXPECT_THAT(LIBC_NAMESPACE::sigismember(&set, 0), Fails(EINVAL, -1));
  EXPECT_THAT(LIBC_NAMESPACE::sigismember(&set, BITS_IN_SIGSET_T + 1),
              Fails(EINVAL, -1));
}
