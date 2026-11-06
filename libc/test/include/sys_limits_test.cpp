//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for sys/limits.h
///
//===----------------------------------------------------------------------===//

#include "test/UnitTest/Test.h"

#include <sys/limits.h>

// Android code includes <sys/limits.h> for the limits <limits.h> has.
TEST(LlvmLibcSysLimitsTest, ProvidesLimitsH) {
  EXPECT_EQ(static_cast<long long>(PATH_MAX), 4096LL);
  EXPECT_EQ(static_cast<long long>(NAME_MAX), 255LL);
  EXPECT_EQ(static_cast<long long>(INT_MAX), 2147483647LL);
  EXPECT_EQ(static_cast<long long>(UID_MAX), 4294967295LL);
  EXPECT_EQ(static_cast<long long>(GID_MAX), 4294967295LL);
}
