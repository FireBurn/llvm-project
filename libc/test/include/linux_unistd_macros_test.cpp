//===-- Unittests for the linux/unistd macros ---------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for the values of the linux unistd macros.
///
//===----------------------------------------------------------------------===//

#include "test/UnitTest/Test.h"

#include "include/llvm-libc-macros/linux/unistd-macros.h"

TEST(LlvmLibcLinuxUnistdMacrosTest, Define_Posix_VersionAndTheSupportedOptio) {
  EXPECT_EQ(static_cast<long long>(_POSIX2_VERSION), 0x316a5LL);
  EXPECT_EQ(static_cast<long long>(_XOPEN_VERSION), 800LL);
  EXPECT_EQ(static_cast<long long>(_POSIX_CHOWN_RESTRICTED), 1LL);
  EXPECT_EQ(static_cast<long long>(_POSIX_JOB_CONTROL), 1LL);
  EXPECT_EQ(static_cast<long long>(_POSIX_SAVED_IDS), 1LL);
  EXPECT_EQ(static_cast<long long>(_POSIX_MAPPED_FILES), 0x316a5LL);
  EXPECT_EQ(static_cast<long long>(_POSIX_MEMORY_PROTECTION), 0x316a5LL);
  EXPECT_EQ(static_cast<long long>(_POSIX_FSYNC), 0x316a5LL);
  EXPECT_EQ(static_cast<long long>(_POSIX_SYNCHRONIZED_IO), 0x316a5LL);
  EXPECT_EQ(static_cast<long long>(_POSIX_TIMERS), 0x316a5LL);
  EXPECT_EQ(static_cast<long long>(_POSIX_MONOTONIC_CLOCK), 0x316a5LL);
  EXPECT_EQ(static_cast<long long>(_POSIX_CLOCK_SELECTION), 0x316a5LL);
  EXPECT_EQ(static_cast<long long>(_POSIX_SEMAPHORES), 0x316a5LL);
  EXPECT_EQ(static_cast<long long>(_POSIX_TIMEOUTS), 0x316a5LL);
  EXPECT_EQ(static_cast<long long>(_POSIX_SHELL), 1LL);
  EXPECT_EQ(static_cast<long long>(_POSIX_PIPE_BUF), 512LL);
  EXPECT_EQ(static_cast<long long>(_POSIX_NO_TRUNC), 1LL);
  EXPECT_EQ(static_cast<long long>(_POSIX_VDISABLE), 0LL);
}
