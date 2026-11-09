//===-- Unittests for sigandset -------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/signal/sigaddset.h"
#include "src/signal/sigandset.h"
#include "src/signal/sigemptyset.h"
#include "src/signal/sigismember.h"

#include "hdr/signal_macros.h"
#include "hdr/types/sigset_t.h"
#include "test/UnitTest/ErrnoSetterMatcher.h"
#include "test/UnitTest/Test.h"

TEST(LlvmLibcSignalTest, SigandsetInvalid) {
  using LIBC_NAMESPACE::testing::ErrnoSetterMatcher::Fails;
  sigset_t a, b, d;
  LIBC_NAMESPACE::sigemptyset(&a);
  LIBC_NAMESPACE::sigemptyset(&b);
  EXPECT_THAT(LIBC_NAMESPACE::sigandset(nullptr, &a, &b), Fails(EINVAL));
  EXPECT_THAT(LIBC_NAMESPACE::sigandset(&d, nullptr, &b), Fails(EINVAL));
  EXPECT_THAT(LIBC_NAMESPACE::sigandset(&d, &a, nullptr), Fails(EINVAL));
}

TEST(LlvmLibcSignalTest, SigandsetCombines) {
  using LIBC_NAMESPACE::testing::ErrnoSetterMatcher::Succeeds;
  sigset_t a, b, d;
  LIBC_NAMESPACE::sigemptyset(&a);
  LIBC_NAMESPACE::sigemptyset(&b);
  LIBC_NAMESPACE::sigaddset(&a, SIGUSR1);
  LIBC_NAMESPACE::sigaddset(&a, SIGINT);
  LIBC_NAMESPACE::sigaddset(&b, SIGUSR1);
  LIBC_NAMESPACE::sigaddset(&b, SIGTERM);

  EXPECT_THAT(LIBC_NAMESPACE::sigandset(&d, &a, &b), Succeeds());
  EXPECT_EQ(LIBC_NAMESPACE::sigismember(&d, SIGUSR1), 1);
  EXPECT_EQ(LIBC_NAMESPACE::sigismember(&d, SIGINT), 0);
  EXPECT_EQ(LIBC_NAMESPACE::sigismember(&d, SIGTERM), 0);
  EXPECT_EQ(LIBC_NAMESPACE::sigismember(&d, SIGHUP), 0);
}

TEST(LlvmLibcSignalTest, SigandsetMayWriteOverAnOperand) {
  using LIBC_NAMESPACE::testing::ErrnoSetterMatcher::Succeeds;
  sigset_t a, b;
  LIBC_NAMESPACE::sigemptyset(&a);
  LIBC_NAMESPACE::sigemptyset(&b);
  LIBC_NAMESPACE::sigaddset(&a, SIGINT);
  LIBC_NAMESPACE::sigaddset(&b, SIGTERM);
  EXPECT_THAT(LIBC_NAMESPACE::sigandset(&a, &a, &b), Succeeds());
  EXPECT_EQ(LIBC_NAMESPACE::sigismember(&a, SIGINT), 0);
  EXPECT_EQ(LIBC_NAMESPACE::sigismember(&a, SIGTERM), 0);
}
