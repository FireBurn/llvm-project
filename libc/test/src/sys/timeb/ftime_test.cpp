//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for ftime
///
//===----------------------------------------------------------------------===//

#include "hdr/types/struct_timeb.h"
#include "src/sys/timeb/ftime.h"
#include "src/time/time_func.h"
#include "test/UnitTest/ErrnoCheckingTest.h"
#include "test/UnitTest/Test.h"

using LlvmLibcFtimeTest = LIBC_NAMESPACE::testing::ErrnoCheckingTest;

TEST_F(LlvmLibcFtimeTest, ReportsTheCurrentTime) {
  time_t before = LIBC_NAMESPACE::time(nullptr);
  struct timeb tb;
  ASSERT_EQ(LIBC_NAMESPACE::ftime(&tb), 0);
  time_t after = LIBC_NAMESPACE::time(nullptr);
  EXPECT_GE(tb.time, before);
  EXPECT_LE(tb.time, after);
  EXPECT_LT(static_cast<int>(tb.millitm), 1000);
}
