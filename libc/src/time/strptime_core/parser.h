//===-- Conversions for strptime --------------------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC_TIME_STRPTIME_CORE_PARSER_H
#define LLVM_LIBC_SRC_TIME_STRPTIME_CORE_PARSER_H

#include "hdr/types/struct_tm.h"
#include "hdr/types/time_t.h"
#include "src/__support/CPP/string_view.h"
#include "src/__support/ctype_utils.h"
#include "src/__support/macros/config.h"
#include "src/time/time_constants.h"
#include "src/time/time_utils.h"

namespace LIBC_NAMESPACE_DECL {
namespace strptime_core {

// Cumulative days before each month, for a common year and a leap year. The
// thirteenth entry is the length of the year, which the searches below rely
// on as a stop.
constexpr int DAYS_BEFORE_MONTH[2][13] = {
    {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365},
    {0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335, 366}};

LIBC_INLINE constexpr bool is_leap(int year) {
  return (year % 4 == 0 && year % 100 != 0) || year % 400 == 0;
}

// What was seen so far, which decides what can be worked out at the end.
struct ParseState {
  bool have_I = false;
  bool have_wday = false;
  bool have_yday = false;
  bool have_mon = false;
  bool have_mday = false;
  bool have_uweek = false;
  bool have_wweek = false;
  bool want_century = false;
  bool want_xday = false;
  bool is_pm = false;
  int century = -1;
  int week_no = 0;
};

// January 1st 1970 was a Thursday; every other day of the week follows from
// how far the date is from it.
LIBC_INLINE void day_of_the_week(struct tm *tm) {
  const int corrected = 1900 + tm->tm_year - (tm->tm_mon < 2 ? 1 : 0);
  int quarters = corrected / 4;
  const int wday = -473 + (365 * (tm->tm_year - 70)) + quarters -
                   (quarters / 25) + ((quarters % 25 < 0) ? 1 : 0) +
                   ((quarters / 25) / 4) + DAYS_BEFORE_MONTH[0][tm->tm_mon] +
                   tm->tm_mday - 1;
  tm->tm_wday = ((wday % 7) + 7) % 7;
}

LIBC_INLINE void day_of_the_year(struct tm *tm) {
  tm->tm_yday =
      DAYS_BEFORE_MONTH[is_leap(1900 + tm->tm_year) ? 1 : 0][tm->tm_mon] +
      tm->tm_mday - 1;
}

// Case insensitive match of one of `names` at `rp`, longest first so that a
// full name is never mistaken for its own abbreviation.
LIBC_INLINE bool match_name(const char *&rp, const cpp::string_view *names,
                            size_t count, int &out) {
  for (size_t i = 0; i < count; ++i) {
    const cpp::string_view name = names[i];
    size_t n = 0;
    for (; n < name.size(); ++n)
      if (internal::tolower(rp[n]) != internal::tolower(name[n]))
        break;
    if (n == name.size()) {
      rp += n;
      out = static_cast<int>(i);
      return true;
    }
  }
  return false;
}

// Reads at most `digits` digits, after any leading spaces, and requires the
// result to be within range. Returns false without moving `rp` past anything
// useful if there is no number there or it is out of range.
LIBC_INLINE bool get_number(const char *&rp, int from, int to, int digits,
                            int &val) {
  while (*rp == ' ')
    ++rp;
  if (!internal::isdigit(*rp))
    return false;
  val = 0;
  int n = digits;
  do {
    val = val * 10 + (*rp++ - '0');
    --n;
  } while (n > 0 && val * 10 <= to && internal::isdigit(*rp));
  return val >= from && val <= to;
}

const char *parse(const char *rp, const char *fmt, struct tm *tm,
                  ParseState &state);

// Runs a nested format, as the composite conversions are defined to do.
LIBC_INLINE bool recurse(const char *&rp, const char *fmt, struct tm *tm,
                         ParseState &state) {
  const char *result = parse(rp, fmt, tm, state);
  if (result == nullptr)
    return false;
  rp = result;
  return true;
}

// Returns where matching stopped in the input, or null if the format did not
// match. `state` accumulates across nested formats.
LIBC_INLINE const char *parse(const char *rp, const char *fmt, struct tm *tm,
                              ParseState &state) {
  while (*fmt != '\0') {
    if (internal::isspace(*fmt)) {
      while (internal::isspace(*rp))
        ++rp;
      ++fmt;
      continue;
    }
    if (*fmt != '%') {
      if (*fmt++ != *rp++)
        return nullptr;
      continue;
    }
    ++fmt;

    // The locale modifiers select alternative representations that the C
    // locale does not have, so they select the ordinary conversion.
    if (*fmt == 'E' || *fmt == 'O')
      ++fmt;

    int val = 0;
    switch (*fmt++) {
    case '%':
      if (*rp++ != '%')
        return nullptr;
      break;
    case 'a':
    case 'A':
      if (!match_name(rp, time_constants::WEEK_DAY_FULL_NAMES.data(),
                      time_constants::DAYS_PER_WEEK, tm->tm_wday) &&
          !match_name(rp, time_constants::WEEK_DAY_NAMES.data(),
                      time_constants::DAYS_PER_WEEK, tm->tm_wday))
        return nullptr;
      state.have_wday = true;
      break;
    case 'b':
    case 'B':
    case 'h':
      if (!match_name(rp, time_constants::MONTH_FULL_NAMES.data(),
                      time_constants::MONTHS_PER_YEAR, tm->tm_mon) &&
          !match_name(rp, time_constants::MONTH_NAMES.data(),
                      time_constants::MONTHS_PER_YEAR, tm->tm_mon))
        return nullptr;
      state.have_mon = true;
      state.want_xday = true;
      break;
    case 'c':
      if (!recurse(rp, "%a %b %e %H:%M:%S %Y", tm, state))
        return nullptr;
      state.want_xday = true;
      break;
    case 'C':
      if (!get_number(rp, 0, 99, 2, val))
        return nullptr;
      state.century = val;
      state.want_xday = true;
      break;
    case 'd':
    case 'e':
      if (!get_number(rp, 1, 31, 2, val))
        return nullptr;
      tm->tm_mday = val;
      state.have_mday = true;
      state.want_xday = true;
      break;
    case 'D':
      if (!recurse(rp, "%m/%d/%y", tm, state))
        return nullptr;
      state.want_xday = true;
      break;
    case 'F':
      if (!recurse(rp, "%Y-%m-%d", tm, state))
        return nullptr;
      state.want_xday = true;
      break;
    case 'H':
      if (!get_number(rp, 0, 23, 2, val))
        return nullptr;
      tm->tm_hour = val;
      state.have_I = false;
      break;
    case 'I':
      if (!get_number(rp, 1, 12, 2, val))
        return nullptr;
      tm->tm_hour = val % 12;
      state.have_I = true;
      break;
    case 'j':
      if (!get_number(rp, 1, 366, 3, val))
        return nullptr;
      tm->tm_yday = val - 1;
      state.have_yday = true;
      break;
    case 'm':
      if (!get_number(rp, 1, 12, 2, val))
        return nullptr;
      tm->tm_mon = val - 1;
      state.have_mon = true;
      state.want_xday = true;
      break;
    case 'M':
      if (!get_number(rp, 0, 59, 2, val))
        return nullptr;
      tm->tm_min = val;
      break;
    case 'n':
    case 't':
      while (internal::isspace(*rp))
        ++rp;
      break;
    case 'p': {
      int index = 0;
      static constexpr cpp::string_view AM_PM[] = {"AM", "PM"};
      if (!match_name(rp, AM_PM, 2, index))
        return nullptr;
      state.is_pm = index == 1;
      break;
    }
    case 'r':
      if (!recurse(rp, "%I:%M:%S %p", tm, state))
        return nullptr;
      break;
    case 'R':
      if (!recurse(rp, "%H:%M", tm, state))
        return nullptr;
      break;
    case 's': {
      // Seconds since the epoch reach well past what a conversion width can
      // hold, so this reads for as long as there are digits.
      if (!internal::isdigit(*rp))
        return nullptr;
      time_t seconds = 0;
      do {
        seconds = seconds * 10 + (*rp++ - '0');
      } while (internal::isdigit(*rp));
      if (!time_utils::localtime_internal(&seconds, tm).has_value())
        return nullptr;
      break;
    }
    case 'S':
      // Sixty and sixty one are allowed, for leap seconds.
      if (!get_number(rp, 0, 61, 2, val))
        return nullptr;
      tm->tm_sec = val;
      break;
    case 'T':
      if (!recurse(rp, "%H:%M:%S", tm, state))
        return nullptr;
      break;
    case 'u':
      if (!get_number(rp, 1, 7, 1, val))
        return nullptr;
      tm->tm_wday = val % 7;
      state.have_wday = true;
      break;
    case 'U':
      if (!get_number(rp, 0, 53, 2, val))
        return nullptr;
      state.week_no = val;
      state.have_uweek = true;
      break;
    case 'w':
      if (!get_number(rp, 0, 6, 1, val))
        return nullptr;
      tm->tm_wday = val;
      state.have_wday = true;
      break;
    case 'W':
      if (!get_number(rp, 0, 53, 2, val))
        return nullptr;
      state.week_no = val;
      state.have_wweek = true;
      break;
    case 'g':
      // The ISO week based year, which says nothing the rest does not.
      if (!get_number(rp, 0, 99, 2, val))
        return nullptr;
      break;
    case 'G':
      if (!internal::isdigit(*rp))
        return nullptr;
      while (internal::isdigit(*rp))
        ++rp;
      break;
    case 'V':
      if (!get_number(rp, 0, 53, 2, val))
        return nullptr;
      break;
    case 'x':
      if (!recurse(rp, "%m/%d/%y", tm, state))
        return nullptr;
      state.want_xday = true;
      break;
    case 'X':
      if (!recurse(rp, "%H:%M:%S", tm, state))
        return nullptr;
      break;
    case 'y':
      if (!get_number(rp, 0, 99, 2, val))
        return nullptr;
      // Two digit years below sixty nine are read as this century, which is
      // the reading the millennium rollover work settled on.
      tm->tm_year = val >= 69 ? val : val + 100;
      state.want_century = true;
      state.want_xday = true;
      break;
    case 'Y':
      if (!get_number(rp, 0, 9999, 4, val))
        return nullptr;
      tm->tm_year = val - 1900;
      state.want_century = false;
      state.want_xday = true;
      break;
    case 'Z':
      // The name of a zone says nothing that can be acted on without a zone
      // database, so it is matched and dropped.
      while (internal::isspace(*rp))
        ++rp;
      while (*rp != '\0' && !internal::isspace(*rp))
        ++rp;
      break;
    case 'z': {
      while (internal::isspace(*rp))
        ++rp;
      if (*rp == 'Z') {
        ++rp;
        tm->tm_gmtoff = 0;
        break;
      }
      if (*rp != '+' && *rp != '-')
        return nullptr;
      const bool negative = *rp++ == '-';
      // Two digits of hours, then optionally a colon and two of minutes.
      int digits = 0;
      val = 0;
      while (digits < 4 && internal::isdigit(*rp)) {
        val = val * 10 + (*rp++ - '0');
        ++digits;
        if (*rp == ':' && digits == 2 && internal::isdigit(rp[1]))
          ++rp;
      }
      if (digits != 2 && digits != 4)
        return nullptr;
      const int hours = digits == 2 ? val : val / 100;
      const int minutes = digits == 2 ? 0 : val % 100;
      if (minutes >= 60)
        return nullptr;
      tm->tm_gmtoff = hours * 3600 + minutes * 60;
      if (negative)
        tm->tm_gmtoff = -tm->tm_gmtoff;
      break;
    }
    default:
      return nullptr;
    }
  }
  return rp;
}

// Works out what the conversions imply but did not state: the hour from a
// twelve hour clock and its meridiem, the year from a century, and the day of
// the week and of the year from the date.
LIBC_INLINE void complete(struct tm *tm, ParseState &state) {
  if (state.have_I && state.is_pm)
    tm->tm_hour += 12;

  if (state.century != -1) {
    if (state.want_century)
      tm->tm_year = tm->tm_year % 100 + (state.century - 19) * 100;
    else
      // A century with no year within it. Odd, but it says something.
      tm->tm_year = (state.century - 19) * 100;
  }

  if (state.want_xday && !state.have_wday) {
    if (!(state.have_mon && state.have_mday) && state.have_yday) {
      const int leap = is_leap(1900 + tm->tm_year) ? 1 : 0;
      int month = 0;
      while (month < 12 && DAYS_BEFORE_MONTH[leap][month] <= tm->tm_yday)
        ++month;
      if (month > 0) {
        if (!state.have_mon)
          tm->tm_mon = month - 1;
        if (!state.have_mday)
          tm->tm_mday = tm->tm_yday - DAYS_BEFORE_MONTH[leap][month - 1] + 1;
        state.have_mon = true;
        state.have_mday = true;
      }
    }
    if (state.have_mon || static_cast<unsigned>(tm->tm_mon) <= 11)
      day_of_the_week(tm);
  }

  if (state.want_xday && !state.have_yday &&
      (state.have_mon || static_cast<unsigned>(tm->tm_mon) <= 11))
    day_of_the_year(tm);

  if ((state.have_uweek || state.have_wweek) && state.have_wday) {
    // A week number counts from the first of the year, so which day that
    // fell on is what the count has to be measured from.
    const int saved_wday = tm->tm_wday;
    const int saved_mday = tm->tm_mday;
    const int saved_mon = tm->tm_mon;
    const int offset = state.have_uweek ? 0 : 1;

    tm->tm_mday = 1;
    tm->tm_mon = 0;
    day_of_the_week(tm);
    if (state.have_mday)
      tm->tm_mday = saved_mday;
    if (state.have_mon)
      tm->tm_mon = saved_mon;

    if (!state.have_yday)
      tm->tm_yday = (7 - (tm->tm_wday - offset)) % 7 + (state.week_no - 1) * 7 +
                    (saved_wday - offset + 7) % 7;

    if (!state.have_mday || !state.have_mon) {
      const int leap = is_leap(1900 + tm->tm_year) ? 1 : 0;
      int month = 0;
      while (month < 12 && DAYS_BEFORE_MONTH[leap][month] <= tm->tm_yday)
        ++month;
      if (month > 0) {
        if (!state.have_mon)
          tm->tm_mon = month - 1;
        if (!state.have_mday)
          tm->tm_mday = tm->tm_yday - DAYS_BEFORE_MONTH[leap][month - 1] + 1;
      }
    }

    tm->tm_wday = saved_wday;
  }
}

} // namespace strptime_core
} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC_TIME_STRPTIME_CORE_PARSER_H
