//===-- Unittests for the spawn macros ---------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for the values of the spawn macros.
///
//===----------------------------------------------------------------------===//

#include "test/UnitTest/Test.h"

#include "include/llvm-libc-macros/spawn-macros.h"

TEST(LlvmLibcSpawnMacrosTest, AddThePosix_Spawn_FlagsToSpawnH) {
  EXPECT_EQ(static_cast<long long>(POSIX_SPAWN_RESETIDS), 1LL);
  EXPECT_EQ(static_cast<long long>(POSIX_SPAWN_SETPGROUP), 2LL);
  EXPECT_EQ(static_cast<long long>(POSIX_SPAWN_SETSIGDEF), 4LL);
  EXPECT_EQ(static_cast<long long>(POSIX_SPAWN_SETSIGMASK), 8LL);
  EXPECT_EQ(static_cast<long long>(POSIX_SPAWN_SETSCHEDPARAM), 16LL);
  EXPECT_EQ(static_cast<long long>(POSIX_SPAWN_SETSCHEDULER), 32LL);
  EXPECT_EQ(static_cast<long long>(POSIX_SPAWN_USEVFORK), 64LL);
  EXPECT_EQ(static_cast<long long>(POSIX_SPAWN_SETSID), 128LL);
}
