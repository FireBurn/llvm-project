//===-- Unittests for the default feature test macros ---------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "include/__llvm-libc-common.h"
#include "test/UnitTest/LibcCTest.h"

// Nothing is asked for here, which is where an application that names none of
// these macros lands. The POSIX interfaces are what it gets, and the X/Open
// ones, not having been asked for, are not claimed.
TEST(DefaultSelectsPosix) {
#ifdef _DEFAULT_SOURCE
  EXPECT_TRUE(_DEFAULT_SOURCE == 1);
#else
  EXPECT_TRUE(0);
#endif
#ifdef _POSIX_SOURCE
  EXPECT_TRUE(_POSIX_SOURCE == 1);
#else
  EXPECT_TRUE(0);
#endif
#ifdef _POSIX_C_SOURCE
  EXPECT_TRUE(_POSIX_C_SOURCE >= 200809L);
#else
  EXPECT_TRUE(0);
#endif
#ifdef _XOPEN_SOURCE
  EXPECT_TRUE(0);
#endif
}
