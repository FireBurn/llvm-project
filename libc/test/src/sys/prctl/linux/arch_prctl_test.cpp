//===-- Unittests for arch_prctl ------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/sys/prctl/arch_prctl.h"
#include "test/UnitTest/ErrnoCheckingTest.h"
#include "test/UnitTest/ErrnoSetterMatcher.h"
#include "test/UnitTest/Test.h"

#include <asm/prctl.h>

using LlvmLibcArchPrctlTest = LIBC_NAMESPACE::testing::ErrnoCheckingTest;
using LIBC_NAMESPACE::testing::ErrnoSetterMatcher::Fails;
using LIBC_NAMESPACE::testing::ErrnoSetterMatcher::Succeeds;

TEST_F(LlvmLibcArchPrctlTest, ReadsTheSegmentBases) {
  unsigned long fs = 0;
  unsigned long gs = 0;
  ASSERT_THAT(LIBC_NAMESPACE::arch_prctl(ARCH_GET_FS, (unsigned long)&fs),
              Succeeds());
  ASSERT_THAT(LIBC_NAMESPACE::arch_prctl(ARCH_GET_GS, (unsigned long)&gs),
              Succeeds());
  // The thread pointer lives in the FS base, so a running program has one.
  ASSERT_NE(fs, 0UL);
}

TEST_F(LlvmLibcArchPrctlTest, SettingTheGsBaseIsReadBack) {
  unsigned long old_gs = 0;
  unsigned long now = 0;
  ASSERT_THAT(LIBC_NAMESPACE::arch_prctl(ARCH_GET_GS, (unsigned long)&old_gs),
              Succeeds());
  ASSERT_THAT(LIBC_NAMESPACE::arch_prctl(ARCH_SET_GS, 0x10000UL), Succeeds());
  ASSERT_THAT(LIBC_NAMESPACE::arch_prctl(ARCH_GET_GS, (unsigned long)&now),
              Succeeds());
  EXPECT_EQ(now, 0x10000UL);
  ASSERT_THAT(LIBC_NAMESPACE::arch_prctl(ARCH_SET_GS, old_gs), Succeeds());
}

TEST_F(LlvmLibcArchPrctlTest, UnknownCodeFails) {
  ASSERT_THAT(LIBC_NAMESPACE::arch_prctl(0x7fffffff, 0), Fails(EINVAL));
}
