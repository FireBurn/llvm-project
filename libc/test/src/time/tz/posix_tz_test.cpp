//===-- Unittests for reading a TZ string ---------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/time/tz/posix_tz.h"
#include "test/UnitTest/Test.h"

using LIBC_NAMESPACE::tz::PosixZone;
using LIBC_NAMESPACE::tz::Rule;

namespace {

PosixZone read(const char *text) {
  PosixZone zone;
  LIBC_NAMESPACE::tz::parse_posix_tz(text, zone);
  return zone;
}

int32_t offset_at(const char *text, time_t seconds) {
  return LIBC_NAMESPACE::tz::answer_for(read(text), seconds).offset;
}

bool daylight_at(const char *text, time_t seconds) {
  return LIBC_NAMESPACE::tz::answer_for(read(text), seconds).daylight;
}

} // anonymous namespace

TEST(LlvmLibcPosixTz, OneZoneNoChanges) {
  PosixZone zone = read("GMT0");
  ASSERT_STREQ(zone.standard_name, "GMT");
  ASSERT_EQ(zone.standard_offset, 0);
  ASSERT_FALSE(zone.has_daylight);
}

TEST(LlvmLibcPosixTz, OffsetIsCountedWestButHeldEast) {
  ASSERT_EQ(read("EST5").standard_offset, -5 * 3600);
  ASSERT_EQ(read("CET-1").standard_offset, 3600);
  ASSERT_EQ(read("IST-5:30").standard_offset, 5 * 3600 + 1800);
  ASSERT_EQ(read("NST3:30:30").standard_offset, -(3 * 3600 + 1830));
}

TEST(LlvmLibcPosixTz, AngleBracketsQuoteAName) {
  PosixZone zone = read("<+0330>-3:30");
  ASSERT_STREQ(zone.standard_name, "+0330");
  ASSERT_EQ(zone.standard_offset, 3 * 3600 + 1800);
}

TEST(LlvmLibcPosixTz, DaylightDefaultsToAnHourAhead) {
  PosixZone zone = read("EST5EDT,M3.2.0,M11.1.0");
  ASSERT_TRUE(zone.has_daylight);
  ASSERT_STREQ(zone.daylight_name, "EDT");
  ASSERT_EQ(zone.daylight_offset, -4 * 3600);
}

TEST(LlvmLibcPosixTz, ChangesAtTwoInTheMorningUnlessTold) {
  PosixZone zone = read("EST5EDT,M3.2.0,M11.1.0");
  ASSERT_EQ(zone.start.seconds, 2 * 3600);
  zone = read("CET-1CEST,M3.5.0,M10.5.0/3");
  ASSERT_EQ(zone.end.seconds, 3 * 3600);
}

TEST(LlvmLibcPosixTz, AChangeMayFallOutsideTheDay) {
  // A zone that changes at midnight at the end of a day states it as the
  // twenty fourth hour of that day.
  PosixZone zone = read("AEST-10AEDT,M10.1.0/24,M4.1.0/3");
  ASSERT_EQ(zone.start.seconds, 24 * 3600);
}

TEST(LlvmLibcPosixTz, ThreeFormsOfSayingWhichDay) {
  ASSERT_EQ(static_cast<int>(read("XXX0YYY,J60,J300").start.form),
            static_cast<int>(Rule::Form::JulianWithoutLeap));
  ASSERT_EQ(static_cast<int>(read("XXX0YYY,59,300").start.form),
            static_cast<int>(Rule::Form::JulianWithLeap));
  ASSERT_EQ(static_cast<int>(read("XXX0YYY,M3.2.0,M11.1.0").start.form),
            static_cast<int>(Rule::Form::MonthWeekDay));
}

TEST(LlvmLibcPosixTz, ANameOnItsOwnStandsAtGreenwich) {
  PosixZone zone = read("leapseconds");
  ASSERT_STREQ(zone.standard_name, "leapseconds");
  ASSERT_EQ(zone.standard_offset, 0);
  ASSERT_FALSE(zone.offset_stated);
}

TEST(LlvmLibcPosixTz, TooShortAName) {
  PosixZone zone = read("AB");
  ASSERT_STREQ(zone.standard_name, "");
  ASSERT_EQ(zone.standard_offset, 0);
}

TEST(LlvmLibcPosixTz, ANameOfAFileIsNotRules) {
  PosixZone zone;
  ASSERT_FALSE(LIBC_NAMESPACE::tz::parse_posix_tz(":Europe/London", zone));
  ASSERT_FALSE(LIBC_NAMESPACE::tz::parse_posix_tz("", zone));
  ASSERT_FALSE(LIBC_NAMESPACE::tz::parse_posix_tz(nullptr, zone));
}

TEST(LlvmLibcPosixTz, NorthernChanges) {
  // Daylight time ran from the tenth of March to the third of November 2024.
  const char *tz = "EST5EDT,M3.2.0,M11.1.0";
  ASSERT_FALSE(daylight_at(tz, 1704110400)); // the first of January
  ASSERT_TRUE(daylight_at(tz, 1719835200));  // the first of July
  ASSERT_EQ(offset_at(tz, 1704110400), -5 * 3600);
  ASSERT_EQ(offset_at(tz, 1719835200), -4 * 3600);
  // Either side of the change forward, which was at two in the morning.
  ASSERT_FALSE(daylight_at(tz, 1710054000 - 1));
  ASSERT_TRUE(daylight_at(tz, 1710054000));
}

TEST(LlvmLibcPosixTz, SouthernChangesRunTheOtherWayRound) {
  const char *tz = "AEST-10AEDT,M10.1.0,M4.1.0/3";
  ASSERT_TRUE(daylight_at(tz, 1704110400));  // the first of January
  ASSERT_FALSE(daylight_at(tz, 1719835200)); // the first of July
  ASSERT_EQ(offset_at(tz, 1704110400), 11 * 3600);
  ASSERT_EQ(offset_at(tz, 1719835200), 10 * 3600);
}

TEST(LlvmLibcPosixTz, YearsBeforeTheEpochUseTheDatesOfNineteenSeventy) {
  // Rules stated this way say nothing about earlier years, so the dates they
  // name in 1970 stand in for all of them. In a northern zone that leaves
  // every earlier moment on standard time.
  const char *tz = "EST5EDT,M3.2.0,M11.1.0";
  ASSERT_FALSE(daylight_at(tz, -1000000000)); // 1938
  ASSERT_FALSE(daylight_at(tz, -60000000));   // 1968
  // A southern zone is on daylight time either side of the new year, so the
  // same dates leave every earlier moment on it.
  const char *southern = "AEST-10AEDT,M10.1.0,M4.1.0/3";
  ASSERT_TRUE(daylight_at(southern, -1000000000));
}

TEST(LlvmLibcPosixTz, TwoZonesAndNoDatesMeansDaylightAllYear) {
  const char *tz = "XXX3YYY";
  ASSERT_TRUE(daylight_at(tz, 0));
  ASSERT_EQ(offset_at(tz, 0), -2 * 3600);
}

// The rules of a zone reach as far as time_t does. Working out which year a
// moment falls in by counting the years from 1970 would take a moment of its
// own at the far end of the range, and the count of days would not fit what
// it was kept in.
TEST(LlvmLibcPosixTz, FarYearsFollowTheSeasons) {
  const char *tz = "EST5EDT,M3.2.0,M11.1.0";
  // Year 54506600, whose months these are.
  constexpr time_t JULY = 1720000010518984LL;
  constexpr time_t NOVEMBER = 1720000021037968LL;
  constexpr time_t JANUARY = 1720000026297460LL;

  ASSERT_TRUE(daylight_at(tz, JULY));
  ASSERT_EQ(offset_at(tz, JULY), -4 * 3600);
  ASSERT_FALSE(daylight_at(tz, NOVEMBER));
  ASSERT_EQ(offset_at(tz, NOVEMBER), -5 * 3600);
  ASSERT_FALSE(daylight_at(tz, JANUARY));
  ASSERT_EQ(offset_at(tz, JANUARY), -5 * 3600);
}

// A zone with no changes in it answers the same wherever it is asked, right
// out to where a signed 64 bit count of seconds runs out.
TEST(LlvmLibcPosixTz, TheEndsOfTimeAreAnswered) {
  const char *fixed = "UTC0";
  ASSERT_EQ(offset_at(fixed, 9223372036854775807LL), 0);
  ASSERT_EQ(offset_at(fixed, -9223372036854775807LL), 0);

  // A zone with changes gives one of its two offsets and nothing else.
  const char *tz = "EST5EDT,M3.2.0,M11.1.0";
  const int32_t far_future = offset_at(tz, 9223372036854775LL);
  ASSERT_TRUE(far_future == -5 * 3600 || far_future == -4 * 3600);
  const int32_t far_past = offset_at(tz, -9223372036854775LL);
  ASSERT_TRUE(far_past == -5 * 3600 || far_past == -4 * 3600);
}
