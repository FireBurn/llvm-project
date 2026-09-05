//===-- Unittests for tzset and the local time it settles -----------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "hdr/types/struct_tm.h"
#include "hdr/types/time_t.h"
#include "src/stdlib/setenv.h"
#include "src/string/strcmp.h"
#include "src/time/localtime_r.h"
#include "src/time/mktime.h"
#include "src/time/tz_variables.h"
#include "src/time/tzset.h"
#include "test/UnitTest/Test.h"

// The rules are given directly rather than by naming a zone, so the test does
// not need the database installed.
namespace {

constexpr const char *EASTERN = "EST5EDT,M3.2.0,M11.1.0";

void use(const char *setting) {
  LIBC_NAMESPACE::setenv("TZ", setting, 1);
  LIBC_NAMESPACE::tzset();
}

struct tm local(time_t seconds) {
  struct tm result;
  LIBC_NAMESPACE::localtime_r(&seconds, &result);
  return result;
}

} // anonymous namespace

TEST(LlvmLibcTzset, GreenwichWhenNothingIsSaid) {
  use("");
  ASSERT_STREQ(LIBC_NAMESPACE::tzname[0], "UTC");
  ASSERT_EQ(LIBC_NAMESPACE::timezone, 0L);
  ASSERT_EQ(LIBC_NAMESPACE::daylight, 0);

  struct tm at_epoch = local(0);
  ASSERT_EQ(at_epoch.tm_year, 70);
  ASSERT_EQ(at_epoch.tm_hour, 0);
  ASSERT_EQ(at_epoch.tm_gmtoff, 0L);
}

TEST(LlvmLibcTzset, ReportsWhatTheRulesSay) {
  use(EASTERN);
  ASSERT_STREQ(LIBC_NAMESPACE::tzname[0], "EST");
  ASSERT_STREQ(LIBC_NAMESPACE::tzname[1], "EDT");
  // timezone counts west, which is the other way round from tm_gmtoff.
  ASSERT_EQ(LIBC_NAMESPACE::timezone, 5L * 3600);
  ASSERT_EQ(LIBC_NAMESPACE::daylight, 1);
}

TEST(LlvmLibcTzset, StandardTime) {
  use(EASTERN);
  // Noon on the first of January 2024, in UT.
  struct tm winter = local(1704110400);
  ASSERT_EQ(winter.tm_year, 124);
  ASSERT_EQ(winter.tm_mon, 0);
  ASSERT_EQ(winter.tm_mday, 1);
  ASSERT_EQ(winter.tm_hour, 7);
  ASSERT_EQ(winter.tm_isdst, 0);
  ASSERT_EQ(winter.tm_gmtoff, -5L * 3600);
  ASSERT_STREQ(winter.tm_zone, "EST");
}

TEST(LlvmLibcTzset, DaylightTime) {
  use(EASTERN);
  // Noon on the first of July 2024, in UT.
  struct tm summer = local(1719835200);
  ASSERT_EQ(summer.tm_hour, 8);
  ASSERT_EQ(summer.tm_isdst, 1);
  ASSERT_EQ(summer.tm_gmtoff, -4L * 3600);
  ASSERT_STREQ(summer.tm_zone, "EDT");
}

TEST(LlvmLibcTzset, EitherSideOfAChange) {
  use(EASTERN);
  // Two in the morning on the tenth of March 2024, when the clocks went
  // forward.
  struct tm before = local(1710054000 - 1);
  ASSERT_EQ(before.tm_isdst, 0);
  ASSERT_EQ(before.tm_hour, 1);
  struct tm after = local(1710054000);
  ASSERT_EQ(after.tm_isdst, 1);
  ASSERT_EQ(after.tm_hour, 3);
}

TEST(LlvmLibcTzset, TurningALocalTimeBackIntoAMoment) {
  use(EASTERN);
  struct tm stated = {};
  stated.tm_year = 124;
  stated.tm_mon = 6;
  stated.tm_mday = 1;
  stated.tm_hour = 8;
  stated.tm_isdst = -1;
  ASSERT_EQ(LIBC_NAMESPACE::mktime(&stated), static_cast<time_t>(1719835200));
  ASSERT_EQ(stated.tm_isdst, 1);
  ASSERT_EQ(stated.tm_gmtoff, -4L * 3600);
}

TEST(LlvmLibcTzset, ARoundTripThroughEveryMonth) {
  use(EASTERN);
  // Noon on the first of each month of 2024.
  time_t moment = 1704110400;
  for (int month = 0; month < 12; ++month) {
    struct tm broken = local(moment);
    broken.tm_isdst = -1;
    ASSERT_EQ(LIBC_NAMESPACE::mktime(&broken), moment);
    moment += 31 * 86400;
  }
}

TEST(LlvmLibcTzset, AStatedTimeThatHappensTwice) {
  use(EASTERN);
  // Half past one on the third of November 2024, which came round twice when
  // the clocks went back.
  struct tm stated = {};
  stated.tm_year = 124;
  stated.tm_mon = 10;
  stated.tm_mday = 3;
  stated.tm_hour = 1;
  stated.tm_min = 30;

  stated.tm_isdst = 1;
  const time_t first = LIBC_NAMESPACE::mktime(&stated);
  ASSERT_EQ(stated.tm_gmtoff, -4L * 3600);

  stated.tm_year = 124;
  stated.tm_mon = 10;
  stated.tm_mday = 3;
  stated.tm_hour = 1;
  stated.tm_min = 30;
  stated.tm_sec = 0;
  stated.tm_isdst = 0;
  const time_t second = LIBC_NAMESPACE::mktime(&stated);
  ASSERT_EQ(stated.tm_gmtoff, -5L * 3600);

  ASSERT_EQ(second - first, static_cast<time_t>(3600));
}

TEST(LlvmLibcTzset, ReadingItAgainPicksUpAChange) {
  use(EASTERN);
  ASSERT_STREQ(LIBC_NAMESPACE::tzname[0], "EST");
  use("CET-1CEST,M3.5.0,M10.5.0/3");
  ASSERT_STREQ(LIBC_NAMESPACE::tzname[0], "CET");
  ASSERT_EQ(LIBC_NAMESPACE::timezone, -3600L);
  struct tm summer = local(1719835200);
  ASSERT_EQ(summer.tm_gmtoff, 2L * 3600);
  ASSERT_STREQ(summer.tm_zone, "CEST");
}

TEST(LlvmLibcTzset, ANameThatGoesNowhereStandsAtGreenwich) {
  use("XYZ");
  ASSERT_STREQ(LIBC_NAMESPACE::tzname[0], "XYZ");
  ASSERT_EQ(LIBC_NAMESPACE::timezone, 0L);
  ASSERT_EQ(local(0).tm_gmtoff, 0L);
}
