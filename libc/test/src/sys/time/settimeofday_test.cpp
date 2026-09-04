//===-- Unittests for settimeofday ----------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "hdr/errno_macros.h"
#include "hdr/types/struct_timeval.h"
#include "src/sys/time/gettimeofday.h"
#include "src/sys/time/settimeofday.h"
#include "test/UnitTest/ErrnoCheckingTest.h"
#include "test/UnitTest/ErrnoSetterMatcher.h"
#include "test/UnitTest/Test.h"

using namespace LIBC_NAMESPACE::testing::ErrnoSetterMatcher;
using LlvmLibcSetTimeOfDayTest = LIBC_NAMESPACE::testing::ErrnoCheckingTest;

TEST_F(LlvmLibcSetTimeOfDayTest, ATimezoneIsRejected) {
  struct timeval tv;
  ASSERT_THAT(LIBC_NAMESPACE::gettimeofday(&tv, nullptr), Succeeds(0));
  int tz = 0;
  // The kernel stopped keeping a timezone, so anything but a null pointer
  // is an error rather than something quietly ignored.
  EXPECT_THAT(LIBC_NAMESPACE::settimeofday(&tv, &tz), Fails(EINVAL, -1));
}

TEST_F(LlvmLibcSetTimeOfDayTest, AnImpossibleTimeIsRejected) {
  struct timeval tv;
  ASSERT_THAT(LIBC_NAMESPACE::gettimeofday(&tv, nullptr), Succeeds(0));

  // A microsecond count of a second or more is not a time of day, and is
  // caught here rather than passed to the kernel.
  struct timeval bad = tv;
  bad.tv_usec = 1000000;
  EXPECT_THAT(LIBC_NAMESPACE::settimeofday(&bad, nullptr), Fails(EINVAL, -1));
  bad.tv_usec = -1;
  EXPECT_THAT(LIBC_NAMESPACE::settimeofday(&bad, nullptr), Fails(EINVAL, -1));
}

TEST_F(LlvmLibcSetTimeOfDayTest, NullTimeIsNothingToDo) {
  EXPECT_THAT(LIBC_NAMESPACE::settimeofday(nullptr, nullptr), Succeeds(0));
}

TEST_F(LlvmLibcSetTimeOfDayTest, SettingTheClockNeedsPrivilege) {
  struct timeval tv;
  ASSERT_THAT(LIBC_NAMESPACE::gettimeofday(&tv, nullptr), Succeeds(0));
  // Setting it to what it already is still needs CAP_SYS_TIME, which the
  // test does not have.
  int ret = LIBC_NAMESPACE::settimeofday(&tv, nullptr);
  if (ret != 0)
    ASSERT_ERRNO_EQ(EPERM);
}
