//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// This file contains unit tests for strptime.
///
//===----------------------------------------------------------------------===//

#include "hdr/types/struct_tm.h"
#include "src/string/memset.h"
#include "src/time/strptime.h"
#include "test/UnitTest/Test.h"

namespace {

struct tm cleared() {
  struct tm time;
  LIBC_NAMESPACE::memset(&time, 0, sizeof(time));
  return time;
}

} // anonymous namespace

TEST(LlvmLibcStrptimeTest, DateAndTime) {
  struct tm time = cleared();
  char *result = LIBC_NAMESPACE::strptime("2026-01-02 03:04:05",
                                          "%Y-%m-%d %H:%M:%S", &time);
  ASSERT_TRUE(result != nullptr);
  EXPECT_EQ(*result, '\0');
  EXPECT_EQ(time.tm_year, 126);
  EXPECT_EQ(time.tm_mon, 0);
  EXPECT_EQ(time.tm_mday, 2);
  EXPECT_EQ(time.tm_hour, 3);
  EXPECT_EQ(time.tm_min, 4);
  EXPECT_EQ(time.tm_sec, 5);
  // Neither was given, and both follow from the date.
  EXPECT_EQ(time.tm_wday, 5);
  EXPECT_EQ(time.tm_yday, 1);
}

TEST(LlvmLibcStrptimeTest, CompositeConversions) {
  struct tm time = cleared();
  ASSERT_TRUE(LIBC_NAMESPACE::strptime("2026-01-02 03:04:05", "%F %T", &time) !=
              nullptr);
  EXPECT_EQ(time.tm_year, 126);
  EXPECT_EQ(time.tm_mday, 2);
  EXPECT_EQ(time.tm_sec, 5);

  time = cleared();
  ASSERT_TRUE(LIBC_NAMESPACE::strptime("Fri Jan  2 03:04:05 2026", "%c",
                                       &time) != nullptr);
  EXPECT_EQ(time.tm_year, 126);
  EXPECT_EQ(time.tm_hour, 3);
}

TEST(LlvmLibcStrptimeTest, Names) {
  struct tm time = cleared();
  // A full name is matched in preference to the abbreviation it starts with.
  ASSERT_TRUE(LIBC_NAMESPACE::strptime("January", "%b", &time) != nullptr);
  EXPECT_EQ(time.tm_mon, 0);

  time = cleared();
  ASSERT_TRUE(LIBC_NAMESPACE::strptime("wednesday", "%A", &time) != nullptr);
  EXPECT_EQ(time.tm_wday, 3);
}

TEST(LlvmLibcStrptimeTest, TwelveHourClock) {
  struct tm time = cleared();
  ASSERT_TRUE(LIBC_NAMESPACE::strptime("11:30:00 PM", "%I:%M:%S %p", &time) !=
              nullptr);
  EXPECT_EQ(time.tm_hour, 23);

  time = cleared();
  ASSERT_TRUE(LIBC_NAMESPACE::strptime("12:30:00 AM", "%I:%M:%S %p", &time) !=
              nullptr);
  EXPECT_EQ(time.tm_hour, 0);
}

TEST(LlvmLibcStrptimeTest, TwoDigitYearAndCentury) {
  struct tm time = cleared();
  ASSERT_TRUE(LIBC_NAMESPACE::strptime("68", "%y", &time) != nullptr);
  EXPECT_EQ(time.tm_year, 168);

  time = cleared();
  ASSERT_TRUE(LIBC_NAMESPACE::strptime("69", "%y", &time) != nullptr);
  EXPECT_EQ(time.tm_year, 69);

  // An explicit century overrides that reading.
  time = cleared();
  ASSERT_TRUE(LIBC_NAMESPACE::strptime("20 26", "%C %y", &time) != nullptr);
  EXPECT_EQ(time.tm_year, 126);
}

TEST(LlvmLibcStrptimeTest, DayOfYearFillsInTheDate) {
  struct tm time = cleared();
  ASSERT_TRUE(LIBC_NAMESPACE::strptime("2024 060", "%Y %j", &time) != nullptr);
  EXPECT_EQ(time.tm_mon, 1);
  EXPECT_EQ(time.tm_mday, 29); // 2024 is a leap year.

  time = cleared();
  ASSERT_TRUE(LIBC_NAMESPACE::strptime("2023 060", "%Y %j", &time) != nullptr);
  EXPECT_EQ(time.tm_mon, 2);
  EXPECT_EQ(time.tm_mday, 1);
}

TEST(LlvmLibcStrptimeTest, ZoneOffset) {
  struct tm time = cleared();
  ASSERT_TRUE(LIBC_NAMESPACE::strptime("+0130", "%z", &time) != nullptr);
  EXPECT_EQ(time.tm_gmtoff, static_cast<long>(5400));

  time = cleared();
  ASSERT_TRUE(LIBC_NAMESPACE::strptime("-05:30", "%z", &time) != nullptr);
  EXPECT_EQ(time.tm_gmtoff, static_cast<long>(-19800));

  time = cleared();
  ASSERT_TRUE(LIBC_NAMESPACE::strptime("Z", "%z", &time) != nullptr);
  EXPECT_EQ(time.tm_gmtoff, static_cast<long>(0));

  // Minutes past fifty nine are not minutes.
  time = cleared();
  EXPECT_EQ(LIBC_NAMESPACE::strptime("+0060", "%z", &time),
            static_cast<char *>(nullptr));
}

TEST(LlvmLibcStrptimeTest, WhitespaceAndLiterals) {
  struct tm time = cleared();
  // Whitespace in the format matches any run of it, including none.
  ASSERT_TRUE(LIBC_NAMESPACE::strptime("2026   01", "%Y %m", &time) != nullptr);
  EXPECT_EQ(time.tm_year, 126);
  EXPECT_EQ(time.tm_mon, 0);

  // A literal that does not match stops the parse.
  time = cleared();
  EXPECT_EQ(LIBC_NAMESPACE::strptime("b2026", "a%Y", &time),
            static_cast<char *>(nullptr));
}

TEST(LlvmLibcStrptimeTest, RejectsOutOfRange) {
  struct tm time = cleared();
  EXPECT_EQ(LIBC_NAMESPACE::strptime("13", "%m", &time),
            static_cast<char *>(nullptr));
  EXPECT_EQ(LIBC_NAMESPACE::strptime("32", "%d", &time),
            static_cast<char *>(nullptr));
  EXPECT_EQ(LIBC_NAMESPACE::strptime("24", "%H", &time),
            static_cast<char *>(nullptr));
  EXPECT_EQ(LIBC_NAMESPACE::strptime("62", "%S", &time),
            static_cast<char *>(nullptr));
  EXPECT_EQ(LIBC_NAMESPACE::strptime("367", "%j", &time),
            static_cast<char *>(nullptr));
  EXPECT_EQ(LIBC_NAMESPACE::strptime("xyz", "%Y", &time),
            static_cast<char *>(nullptr));
}

TEST(LlvmLibcStrptimeTest, ReturnsWhereItStopped) {
  struct tm time = cleared();
  char *result =
      LIBC_NAMESPACE::strptime("2026-01-02 trailing", "%Y-%m-%d", &time);
  ASSERT_TRUE(result != nullptr);
  EXPECT_EQ(*result, ' ');
}

TEST(LlvmLibcStrptimeTest, LocaleModifiersSelectTheOrdinaryConversion) {
  struct tm time = cleared();
  ASSERT_TRUE(LIBC_NAMESPACE::strptime("2026", "%EY", &time) != nullptr);
  EXPECT_EQ(time.tm_year, 126);

  time = cleared();
  ASSERT_TRUE(LIBC_NAMESPACE::strptime("01", "%Om", &time) != nullptr);
  EXPECT_EQ(time.tm_mon, 0);
}
