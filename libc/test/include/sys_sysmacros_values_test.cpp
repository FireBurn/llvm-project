//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for major, minor and makedev expanding to what they mean
///
//===----------------------------------------------------------------------===//

#include "include/sys/sysmacros.h"
#include "test/UnitTest/Test.h"

TEST(LlvmLibcSysSysmacrosValuesTest, MakedevRoundTrips) {
  dev_t device = makedev(8, 17);
  EXPECT_EQ(major(device), 8u);
  EXPECT_EQ(minor(device), 17u);
  dev_t big = makedev(0x1234, 0x56789);
  EXPECT_EQ(major(big), 0x1234u);
  EXPECT_EQ(minor(big), 0x56789u);
}
