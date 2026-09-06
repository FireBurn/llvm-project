//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for sys/types.h bringing in fd_set
///
//===----------------------------------------------------------------------===//

#include "include/sys/types.h"
#include "test/UnitTest/Test.h"

TEST(LlvmLibcSysTypesFdSetTest, FdSetIsUsableFromSysTypes) {
  fd_set set;
  FD_ZERO(&set);
  EXPECT_EQ(static_cast<long long>(FD_ISSET(3, &set)), 0LL);
  FD_SET(3, &set);
  EXPECT_NE(static_cast<long long>(FD_ISSET(3, &set)), 0LL);
  FD_CLR(3, &set);
  EXPECT_EQ(static_cast<long long>(FD_ISSET(3, &set)), 0LL);
}
