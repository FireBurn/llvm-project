//===-- Unittests for swapon and swapoff ----------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "hdr/errno_macros.h"
#include "hdr/sys_swap_macros.h"
#include "src/sys/swap/swapoff.h"
#include "src/sys/swap/swapon.h"
#include "test/UnitTest/ErrnoCheckingTest.h"
#include "test/UnitTest/ErrnoSetterMatcher.h"
#include "test/UnitTest/Test.h"

using namespace LIBC_NAMESPACE::testing::ErrnoSetterMatcher;
using LlvmLibcSwapTest = LIBC_NAMESPACE::testing::ErrnoCheckingTest;

// Turning swap on needs CAP_SYS_ADMIN, which the tests do not have, so what
// is checked is that the call reaches the kernel and comes back with the
// right refusal rather than something of the libc's own making.

TEST_F(LlvmLibcSwapTest, NeedsPrivilege) {
  EXPECT_THAT(LIBC_NAMESPACE::swapon("/dev/null", 0),
              Fails(any_of(EPERM, EINVAL), -1));
  EXPECT_THAT(LIBC_NAMESPACE::swapoff("/dev/null"),
              Fails(any_of(EPERM, EINVAL), -1));
}

TEST_F(LlvmLibcSwapTest, MissingPath) {
  EXPECT_THAT(LIBC_NAMESPACE::swapon("/no/such/swap/file", 0),
              Fails(any_of(EPERM, ENOENT), -1));
  EXPECT_THAT(LIBC_NAMESPACE::swapoff("/no/such/swap/file"),
              Fails(any_of(EPERM, ENOENT), -1));
}

TEST_F(LlvmLibcSwapTest, PriorityFlagsAreAccepted) {
  // The flags go through untouched; the kernel is what rejects the call.
  int flags = SWAP_FLAG_PREFER | (5 << SWAP_FLAG_PRIO_SHIFT);
  EXPECT_THAT(LIBC_NAMESPACE::swapon("/dev/null", flags),
              Fails(any_of(EPERM, EINVAL), -1));
}
