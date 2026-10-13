//===-- Unittests for the malloc macros ---------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for the values of the malloc macros.
///
//===----------------------------------------------------------------------===//

#include "test/UnitTest/Test.h"

#include "include/llvm-libc-macros/malloc-macros.h"

TEST(LlvmLibcMallocMacrosTest, DefineTheMalloptParametersGlibcSMallocHD) {
  EXPECT_EQ(static_cast<long long>(M_MXFAST), 1LL);
  EXPECT_EQ(static_cast<long long>(M_NLBLKS), 2LL);
  EXPECT_EQ(static_cast<long long>(M_GRAIN), 3LL);
  EXPECT_EQ(static_cast<long long>(M_KEEP), 4LL);
  EXPECT_EQ(static_cast<long long>(M_TRIM_THRESHOLD), -1LL);
  EXPECT_EQ(static_cast<long long>(M_TOP_PAD), -2LL);
  EXPECT_EQ(static_cast<long long>(M_MMAP_THRESHOLD), -3LL);
  EXPECT_EQ(static_cast<long long>(M_MMAP_MAX), -4LL);
  EXPECT_EQ(static_cast<long long>(M_CHECK_ACTION), -5LL);
  EXPECT_EQ(static_cast<long long>(M_PERTURB), -6LL);
  EXPECT_EQ(static_cast<long long>(M_ARENA_TEST), -7LL);
  EXPECT_EQ(static_cast<long long>(M_ARENA_MAX), -8LL);
}
