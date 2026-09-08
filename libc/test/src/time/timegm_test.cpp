//===-- Unittests for timegm ----------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "hdr/types/struct_tm.h"
#include "hdr/types/time_t.h"
#include "src/time/gmtime_r.h"
#include "src/time/timegm.h"
#include "test/UnitTest/Test.h"

namespace {

struct tm broken_down(int year, int month, int day, int hour, int minute,
                      int second) {
  struct tm t = {};
  t.tm_year = year - 1900;
  t.tm_mon = month - 1;
  t.tm_mday = day;
  t.tm_hour = hour;
  t.tm_min = minute;
  t.tm_sec = second;
  return t;
}

} // anonymous namespace

// timegm reads the fields as a time at Greenwich whatever zone the machine
// keeps, which is the whole of what separates it from mktime.
TEST(LlvmLibcTimegmTest, TheStartOfTheEpoch) {
  struct tm t = broken_down(1970, 1, 1, 0, 0, 0);
  ASSERT_EQ(LIBC_NAMESPACE::timegm(&t), time_t(0));
}

TEST(LlvmLibcTimegmTest, ADateWithinTheEpoch) {
  struct tm t = broken_down(2001, 9, 9, 1, 46, 40);
  ASSERT_EQ(LIBC_NAMESPACE::timegm(&t), time_t(1000000000));
}

// The day of the week and of the year are filled in from the rest, and
// fields outside their range are carried into the ones above them.
TEST(LlvmLibcTimegmTest, FillsInTheRestAndCarriesTheOverflow) {
  struct tm t = broken_down(2024, 2, 29, 12, 0, 0);
  time_t when = LIBC_NAMESPACE::timegm(&t);
  ASSERT_EQ(t.tm_wday, 4); // A Thursday.
  ASSERT_EQ(t.tm_yday, 59);

  struct tm carried = broken_down(2023, 13, 1, 0, 0, 0);
  time_t rolled = LIBC_NAMESPACE::timegm(&carried);
  ASSERT_EQ(carried.tm_year, 2024 - 1900);
  ASSERT_EQ(carried.tm_mon, 0);

  // What it answers reads back as the same fields.
  struct tm again;
  ASSERT_FALSE(LIBC_NAMESPACE::gmtime_r(&when, &again) == nullptr);
  ASSERT_EQ(again.tm_year, 2024 - 1900);
  ASSERT_EQ(again.tm_mon, 1);
  ASSERT_EQ(again.tm_mday, 29);
  ASSERT_FALSE(LIBC_NAMESPACE::gmtime_r(&rolled, &again) == nullptr);
  ASSERT_EQ(again.tm_year, 2024 - 1900);
  ASSERT_EQ(again.tm_mon, 0);
}
