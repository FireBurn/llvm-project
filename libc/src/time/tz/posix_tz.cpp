//===-- The rules a TZ string states --------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/time/tz/posix_tz.h"

#include "src/__support/ctype_utils.h"
#include "src/__support/macros/config.h"
#include "src/time/time_constants.h"
#include "src/time/time_utils.h"

namespace LIBC_NAMESPACE_DECL {
namespace tz {

namespace {

constexpr int32_t SECONDS_PER_DAY = 86400;

// Reads a run of digits, at most `limit` of them, and reports whether there
// were any.
bool read_number(const char *&at, int &out, int limit) {
  if (!internal::isdigit(*at))
    return false;
  out = 0;
  int taken = 0;
  while (internal::isdigit(*at) && taken < limit) {
    out = out * 10 + (*at++ - '0');
    ++taken;
  }
  return true;
}

// Reads the name a zone goes by: three or more letters, or anything at all
// between angle brackets.
bool read_name(const char *&at, char *out) {
  size_t n = 0;
  if (*at == '<') {
    ++at;
    while (*at != '>' && *at != '\0') {
      if (n >= MAX_ABBREVIATION)
        return false;
      out[n++] = *at++;
    }
    if (*at != '>')
      return false;
    ++at;
  } else {
    while (internal::isalpha(*at)) {
      if (n >= MAX_ABBREVIATION)
        return false;
      out[n++] = *at++;
    }
  }
  if (n < 3)
    return false;
  out[n] = '\0';
  return true;
}

// Reads hh[:mm[:ss]] with an optional sign, as a count of seconds.
//
// `wide` says whether the hours may run past 24, which they may where the
// string says when in the day a change happens but not where it states an
// offset from Greenwich.
bool read_time(const char *&at, int32_t &out, bool wide) {
  bool negative = false;
  if (*at == '+')
    ++at;
  else if (*at == '-') {
    negative = true;
    ++at;
  }
  int hours = 0;
  if (!read_number(at, hours, wide ? 3 : 2))
    return false;
  if (hours > (wide ? 167 : 24))
    return false;
  int minutes = 0;
  int seconds = 0;
  if (*at == ':') {
    ++at;
    if (!read_number(at, minutes, 2) || minutes > 59)
      return false;
    if (*at == ':') {
      ++at;
      if (!read_number(at, seconds, 2) || seconds > 59)
        return false;
    }
  }
  out = hours * 3600 + minutes * 60 + seconds;
  if (negative)
    out = -out;
  return true;
}

bool read_rule(const char *&at, Rule &out) {
  if (*at == 'J') {
    ++at;
    out.form = Rule::Form::JulianWithoutLeap;
    if (!read_number(at, out.day, 3) || out.day < 1 || out.day > 365)
      return false;
  } else if (*at == 'M') {
    ++at;
    out.form = Rule::Form::MonthWeekDay;
    if (!read_number(at, out.month, 2) || out.month < 1 || out.month > 12)
      return false;
    if (*at++ != '.')
      return false;
    if (!read_number(at, out.week, 1) || out.week < 1 || out.week > 5)
      return false;
    if (*at++ != '.')
      return false;
    if (!read_number(at, out.weekday, 1) || out.weekday > 6)
      return false;
  } else {
    out.form = Rule::Form::JulianWithLeap;
    if (!read_number(at, out.day, 3) || out.day > 365)
      return false;
  }

  out.seconds = 2 * 3600;
  if (*at == '/') {
    ++at;
    // The time a change happens is written the same way as an offset but may
    // run outside the day, which is how a zone says a change happens at
    // midnight at the end of a day rather than the start.
    if (!read_time(at, out.seconds, /*wide=*/true))
      return false;
  }
  return true;
}

// Days from the epoch to the first of January of a year. The Gregorian
// calendar repeats on a cycle of 400 years, which is 146097 days, so the
// answer follows from the year rather than from counting the years one at a
// time. A rule may be asked about a year hundreds of millions away, which
// counting would take a moment of its own to reach.
int64_t days_before_year(int64_t year) {
  const int64_t y = year - 1;
  const int64_t era = (y >= 0 ? y : y - 399) / 400;
  const int64_t year_of_era = y - era * 400;
  // 306 is where the first of January sits in a year that starts in March,
  // which is the form the cycle is regular in.
  const int64_t day_of_era =
      year_of_era * 365 + year_of_era / 4 - year_of_era / 100 + 306;
  // 719468 moves the count from the start of the cycle to the epoch.
  return era * 146097 + day_of_era - 719468;
}

// Which day of the year, counting from zero, a rule names.
int day_of_year(const Rule &rule, int year) {
  const bool leap = time_utils::is_leap_year(year);
  switch (rule.form) {
  case Rule::Form::JulianWithoutLeap: {
    // The leap day is not counted, so every date past February is one later
    // in a leap year than the number says.
    int day = rule.day - 1;
    if (leap && rule.day > 59)
      ++day;
    return day;
  }
  case Rule::Form::JulianWithLeap:
    return rule.day;
  case Rule::Form::MonthWeekDay:
    break;
  }

  // Which day of the week the month starts on. The first of January 1970 was
  // a Thursday, and the days since then say the rest.
  const int leap_index = leap ? 1 : 0;
  static constexpr int DAYS_BEFORE_MONTH[2][13] = {
      {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365},
      {0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335, 366}};

  const int64_t first_of_month =
      days_before_year(year) + DAYS_BEFORE_MONTH[leap_index][rule.month - 1];
  // Thursday is four days from Sunday.
  int weekday = static_cast<int>((first_of_month + 4) % 7);
  if (weekday < 0)
    weekday += 7;

  const int days_in_month = DAYS_BEFORE_MONTH[leap_index][rule.month] -
                            DAYS_BEFORE_MONTH[leap_index][rule.month - 1];
  // The first such weekday in the month, then as many weeks on as asked for,
  // stopping at the last one the month has.
  int day = (rule.weekday - weekday + 7) % 7;
  day += (rule.week - 1) * 7;
  while (day >= days_in_month)
    day -= 7;
  return DAYS_BEFORE_MONTH[leap_index][rule.month - 1] + day;
}

// The moment a rule names, as a count of seconds from the epoch in local
// standard time.
time_t moment_of(const Rule &rule, int year, int32_t offset) {
  int64_t days = days_before_year(year) + day_of_year(rule, year);
  return static_cast<time_t>(days) * SECONDS_PER_DAY + rule.seconds - offset;
}

// Which year a moment falls in, near enough to pick the rules with. Being a
// day out at the very edge of a year does not matter: the two rules of a year
// are months apart from its edges.
int year_of(time_t seconds) {
  // Whole days, rounded towards the past so that a moment before the epoch
  // lands in the day it belongs to rather than the one after.
  int64_t days = static_cast<int64_t>(seconds) / SECONDS_PER_DAY;
  if (seconds % SECONDS_PER_DAY < 0)
    --days;

  // The reverse of days_before_year, over the same cycle of 400 years.
  const int64_t shifted = days + 719468;
  const int64_t era = (shifted >= 0 ? shifted : shifted - 146096) / 146097;
  const int64_t day_of_era = shifted - era * 146097;
  const int64_t year_of_era = (day_of_era - day_of_era / 1460 +
                               day_of_era / 36524 - day_of_era / 146096) /
                              365;
  const int64_t day_of_year =
      day_of_era - (365 * year_of_era + year_of_era / 4 - year_of_era / 100);
  // The cycle counts from March, so the last two months of it belong to the
  // year after the one the arithmetic above names.
  const int64_t month = (5 * day_of_year + 2) / 153;
  int64_t year = year_of_era + era * 400 + (month >= 10 ? 1 : 0);
  return static_cast<int>(year);
}

} // anonymous namespace

bool parse_posix_tz(const char *text, PosixZone &out) {
  if (text == nullptr || *text == '\0')
    return false;
  const char *at = text;
  // A leading colon says what follows names a file rather than states rules.
  if (*at == ':')
    return false;

  out = PosixZone{};
  if (!read_name(at, out.standard_name)) {
    out.standard_name[0] = '\0';
    return true;
  }
  int32_t written = 0;
  if (!read_time(at, written, /*wide=*/false))
    return true;
  // The string counts west of Greenwich; everything else counts east.
  out.standard_offset = -written;
  out.offset_stated = true;

  if (*at == '\0') {
    // One zone all year, which reports the same name either way round.
    for (size_t i = 0; i <= MAX_ABBREVIATION; ++i)
      out.daylight_name[i] = out.standard_name[i];
    return true;
  }

  if (!read_name(at, out.daylight_name)) {
    out.daylight_name[0] = '\0';
    return true;
  }
  out.has_daylight = true;
  if (*at == '+' || *at == '-' || internal::isdigit(*at)) {
    if (!read_time(at, written, /*wide=*/false))
      return true;
    out.daylight_offset = -written;
  } else {
    // An hour ahead of standard time where the string does not say.
    out.daylight_offset = out.standard_offset + 3600;
  }

  // Without dates the zone is on daylight time the whole year, which is what
  // a string that names two zones and states no rules means.
  if (*at != ',')
    return true;
  ++at;
  if (!read_rule(at, out.start))
    return true;
  if (*at != ',')
    return true;
  ++at;
  read_rule(at, out.end);
  return true;
}

Answer answer_for(const PosixZone &zone, time_t seconds) {
  if (!zone.has_daylight)
    return {zone.standard_offset, zone.standard_name, false};

  int year = year_of(seconds + zone.standard_offset);
  // Rules stated this way say nothing about the years before the epoch, so
  // the dates they name in 1970 stand in for all of them.
  if (year < 1970)
    year = 1970;

  // Each rule is stated in the time in force just before the change: the one
  // into daylight time in standard time, and the one out of it in daylight
  // time.
  const time_t starts = moment_of(zone.start, year, zone.standard_offset);
  const time_t ends = moment_of(zone.end, year, zone.daylight_offset);

  // A zone in the southern half of the world changes out of daylight time
  // earlier in the year than it changes into it, so the two are the other way
  // round and it is the middle of the year that is standard time.
  const bool daylight = starts <= ends ? (seconds >= starts && seconds < ends)
                                       : (seconds >= starts || seconds < ends);
  if (daylight)
    return {zone.daylight_offset, zone.daylight_name, true};
  return {zone.standard_offset, zone.standard_name, false};
}

} // namespace tz
} // namespace LIBC_NAMESPACE_DECL
