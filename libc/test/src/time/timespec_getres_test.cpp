//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// This file contains the tests of timespec_getres.
///
//===----------------------------------------------------------------------===//

#include "hdr/time_macros.h"
#include "hdr/types/struct_timespec.h"
#include "src/time/timespec_getres.h"
#include "test/UnitTest/ErrnoCheckingTest.h"
#include "test/UnitTest/Test.h"

using LlvmLibcTimespecGetresTest = LIBC_NAMESPACE::testing::ErrnoCheckingTest;

TEST_F(LlvmLibcTimespecGetresTest, ReportsTheResolutionOfEachBase) {
  const int bases[] = {TIME_UTC, TIME_MONOTONIC, TIME_ACTIVE,
                       TIME_THREAD_ACTIVE};
  for (int base : bases) {
    struct timespec res = {-1, -1};
    ASSERT_EQ(LIBC_NAMESPACE::timespec_getres(&res, base), base);
    ASSERT_EQ(res.tv_sec, time_t(0));
    ASSERT_GT(res.tv_nsec, 0L);
    ASSERT_LE(res.tv_nsec, 1000000000L);
  }
}

TEST_F(LlvmLibcTimespecGetresTest, AcceptsNoResultAndRefusesUnknownBases) {
  ASSERT_EQ(LIBC_NAMESPACE::timespec_getres(nullptr, TIME_UTC), TIME_UTC);
  struct timespec res = {-1, -1};
  ASSERT_EQ(LIBC_NAMESPACE::timespec_getres(&res, 0), 0);
  ASSERT_EQ(res.tv_sec, time_t(-1));
}
