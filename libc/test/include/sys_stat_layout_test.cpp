//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for the layout of struct stat
///
//===----------------------------------------------------------------------===//

#include "include/sys/stat.h"
#include "test/UnitTest/Test.h"

// The layouts are the ones glibc has, which programs built against it assume.
TEST(LlvmLibcSysStatLayoutTest, MatchesGlibcOnX86_64) {
#if defined(__x86_64__) && defined(__linux__)
  EXPECT_EQ(sizeof(struct stat), size_t(144));
  EXPECT_EQ(__builtin_offsetof(struct stat, st_dev), size_t(0));
  EXPECT_EQ(__builtin_offsetof(struct stat, st_ino), size_t(8));
  EXPECT_EQ(__builtin_offsetof(struct stat, st_nlink), size_t(16));
  EXPECT_EQ(__builtin_offsetof(struct stat, st_mode), size_t(24));
  EXPECT_EQ(__builtin_offsetof(struct stat, st_uid), size_t(28));
  EXPECT_EQ(__builtin_offsetof(struct stat, st_gid), size_t(32));
  EXPECT_EQ(__builtin_offsetof(struct stat, st_rdev), size_t(40));
  EXPECT_EQ(__builtin_offsetof(struct stat, st_size), size_t(48));
  EXPECT_EQ(__builtin_offsetof(struct stat, st_blksize), size_t(56));
  EXPECT_EQ(__builtin_offsetof(struct stat, st_blocks), size_t(64));
  EXPECT_EQ(__builtin_offsetof(struct stat, st_atim), size_t(72));
  EXPECT_EQ(__builtin_offsetof(struct stat, st_mtim), size_t(88));
  EXPECT_EQ(__builtin_offsetof(struct stat, st_ctim), size_t(104));
#endif
}

// The generic kernel layout, which statx-less calls fill in whole.
TEST(LlvmLibcSysStatLayoutTest, MatchesKernelOnGenericTargets) {
#if (defined(__aarch64__) || defined(__riscv)) && defined(__linux__)
  EXPECT_EQ(sizeof(struct stat), size_t(128));
  EXPECT_EQ(__builtin_offsetof(struct stat, st_mode), size_t(16));
  EXPECT_EQ(__builtin_offsetof(struct stat, st_nlink), size_t(20));
  EXPECT_EQ(__builtin_offsetof(struct stat, st_rdev), size_t(32));
  EXPECT_EQ(__builtin_offsetof(struct stat, st_size), size_t(48));
  EXPECT_EQ(__builtin_offsetof(struct stat, st_blksize), size_t(56));
  EXPECT_EQ(__builtin_offsetof(struct stat, st_blocks), size_t(64));
  EXPECT_EQ(__builtin_offsetof(struct stat, st_atim), size_t(72));
  EXPECT_EQ(__builtin_offsetof(struct stat, st_ctim), size_t(104));
#endif
}
