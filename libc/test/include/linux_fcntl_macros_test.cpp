//===-- Unittests for the linux/fcntl macros ---------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for the values of the linux fcntl macros.
///
//===----------------------------------------------------------------------===//

#include "test/UnitTest/Test.h"

#include "include/llvm-libc-macros/linux/fcntl-macros.h"

TEST(LlvmLibcLinuxFcntlMacrosTest, FillInTheMissingFcntlStatxAndMountMacros) {
  EXPECT_EQ(static_cast<long long>(F_LINUX_SPECIFIC_BASE), 1024LL);
  EXPECT_EQ(static_cast<long long>(F_SETPIPE_SZ), 1031LL);
  EXPECT_EQ(static_cast<long long>(F_GETPIPE_SZ), 1032LL);
  EXPECT_EQ(static_cast<long long>(F_ADD_SEALS), 1033LL);
  EXPECT_EQ(static_cast<long long>(F_GET_SEALS), 1034LL);
  EXPECT_EQ(static_cast<long long>(F_SEAL_SEAL), 1LL);
  EXPECT_EQ(static_cast<long long>(F_SEAL_SHRINK), 2LL);
  EXPECT_EQ(static_cast<long long>(F_SEAL_GROW), 4LL);
  EXPECT_EQ(static_cast<long long>(F_SEAL_WRITE), 8LL);
  EXPECT_EQ(static_cast<long long>(F_SEAL_FUTURE_WRITE), 16LL);
  EXPECT_EQ(static_cast<long long>(F_SEAL_EXEC), 32LL);
  EXPECT_EQ(static_cast<long long>(SPLICE_F_MOVE), 1LL);
  EXPECT_EQ(static_cast<long long>(SPLICE_F_NONBLOCK), 2LL);
  EXPECT_EQ(static_cast<long long>(SPLICE_F_MORE), 4LL);
  EXPECT_EQ(static_cast<long long>(SPLICE_F_GIFT), 8LL);
  EXPECT_EQ(static_cast<long long>(FALLOC_FL_ALLOCATE_RANGE), 0LL);
  EXPECT_EQ(static_cast<long long>(FALLOC_FL_KEEP_SIZE), 1LL);
  EXPECT_EQ(static_cast<long long>(FALLOC_FL_PUNCH_HOLE), 2LL);
  EXPECT_EQ(static_cast<long long>(FALLOC_FL_NO_HIDE_STALE), 4LL);
  EXPECT_EQ(static_cast<long long>(FALLOC_FL_COLLAPSE_RANGE), 8LL);
  EXPECT_EQ(static_cast<long long>(FALLOC_FL_ZERO_RANGE), 16LL);
  EXPECT_EQ(static_cast<long long>(FALLOC_FL_INSERT_RANGE), 32LL);
  EXPECT_EQ(static_cast<long long>(FALLOC_FL_UNSHARE_RANGE), 64LL);
  EXPECT_EQ(static_cast<long long>(FALLOC_FL_WRITE_ZEROES), 128LL);
}

TEST(LlvmLibcLinuxFcntlMacrosTest, StopAskingTheKernelForALockCommandItDoes) {
  EXPECT_EQ(static_cast<long long>(F_GETLK64), 5LL);
  EXPECT_EQ(static_cast<long long>(F_SETLK64), 6LL);
  EXPECT_EQ(static_cast<long long>(F_SETLKW64), 7LL);
}
