//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for sys/time.h bringing in sys/select.h
///
//===----------------------------------------------------------------------===//

#include "include/sys/time.h"
#include "test/UnitTest/Test.h"

TEST(LlvmLibcSysTimeSelectTest, DefinesWhatPosixAsksFor) {
  fd_set set;
  FD_ZERO(&set);
  FD_SET(3, &set);
  EXPECT_EQ(static_cast<long long>(FD_ISSET(3, &set)), 1LL);
  EXPECT_EQ(static_cast<long long>(FD_ISSET(0, &set)), 0LL);
  EXPECT_GT(FD_SETSIZE, 3);

  int (*select_fn)(int, fd_set *, fd_set *, fd_set *, struct timeval *) =
      &select;
  EXPECT_TRUE(select_fn != nullptr);
}
