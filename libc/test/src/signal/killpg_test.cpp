//===-- Unittests for killpg ----------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "hdr/errno_macros.h"
#include "src/signal/killpg.h"
#include "src/unistd/getpgrp.h"
#include "test/UnitTest/ErrnoCheckingTest.h"
#include "test/UnitTest/ErrnoSetterMatcher.h"
#include "test/UnitTest/Test.h"

using LlvmLibcKillpgTest = LIBC_NAMESPACE::testing::ErrnoCheckingTest;
using LIBC_NAMESPACE::testing::ErrnoSetterMatcher::Fails;
using LIBC_NAMESPACE::testing::ErrnoSetterMatcher::Succeeds;

// Signal zero sends nothing and only asks whether the group can be reached,
// which the caller's own group always can. A group of zero is the caller's
// own as well.
TEST_F(LlvmLibcKillpgTest, ReachesTheCallersOwnGroup) {
  ASSERT_THAT(LIBC_NAMESPACE::killpg(LIBC_NAMESPACE::getpgrp(), 0),
              Succeeds(0));
  ASSERT_THAT(LIBC_NAMESPACE::killpg(0, 0), Succeeds(0));
}

// Negating a negative group would turn it into a single process, so it is
// refused rather than passed on.
TEST_F(LlvmLibcKillpgTest, RefusesANegativeGroup) {
  ASSERT_THAT(LIBC_NAMESPACE::killpg(-1, 0), Fails(EINVAL));
}
