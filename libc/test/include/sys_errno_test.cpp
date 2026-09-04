//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for sys/errno.h
///
//===----------------------------------------------------------------------===//

#include "include/sys/errno.h"
#include "test/UnitTest/Test.h"

TEST(LlvmLibcSysErrnoTest, SpellingGivesTheSameCodes) {
  EXPECT_EQ(ENOENT, 2);
  EXPECT_EQ(EINVAL, 22);
}
