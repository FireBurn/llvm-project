//===-- The rules a TZ string states ----------------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC_TIME_TZ_POSIX_TZ_H
#define LLVM_LIBC_SRC_TIME_TZ_POSIX_TZ_H

#include "hdr/stdint_proxy.h"
#include "hdr/types/size_t.h"
#include "hdr/types/time_t.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {
namespace tz {

// The longest abbreviation a zone may go by. The format allows more, but
// nothing real uses more than six and POSIX only promises this much.
constexpr size_t MAX_ABBREVIATION = 16;

// When in the year a change happens, in one of the three forms the format
// allows.
struct Rule {
  enum class Form {
    // Jn: the nth day of the year counting from one, never counting the leap
    // day, so the same date every year.
    JulianWithoutLeap,
    // n: the nth day counting from zero, counting the leap day.
    JulianWithLeap,
    // Mm.w.d: the dth day of the wth week of the mth month, where week five
    // means the last such day in the month.
    MonthWeekDay
  };

  Form form = Form::MonthWeekDay;
  // For the Julian forms, which day. For the third, the month, the week and
  // the day of the week.
  int day = 0;
  int month = 0;
  int week = 0;
  int weekday = 0;
  // How far into that day, in seconds. Two in the morning where the string
  // does not say, and allowed to fall outside the day.
  int32_t seconds = 2 * 3600;
};

// What a TZ string says: one zone, or two and when to change between them.
struct PosixZone {
  char standard_name[MAX_ABBREVIATION + 1] = {};
  char daylight_name[MAX_ABBREVIATION + 1] = {};
  // Seconds east of Greenwich, which is the opposite sign from how the string
  // writes it.
  int32_t standard_offset = 0;
  int32_t daylight_offset = 0;
  bool has_daylight = false;
  // Whether the string went as far as stating an offset. Where it named a
  // zone and stopped, the name stands and the offset is Greenwich.
  bool offset_stated = false;
  Rule start;
  Rule end;
};

// Reads a TZ string. A string that does not hold together is read as far as
// it goes rather than thrown out: the leading name stands on its own if
// nothing follows it, and a string that does not even start with a name
// leaves the zone unnamed at Greenwich. Returns false only for a string that
// names a file instead of stating rules.
bool parse_posix_tz(const char *text, PosixZone &out);

// Which of the two the given time falls in, and what its offset and name are.
// `seconds` is a count from the epoch in UT.
struct Answer {
  int32_t offset;
  const char *name;
  bool daylight;
};

Answer answer_for(const PosixZone &zone, time_t seconds);

} // namespace tz
} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC_TIME_TZ_POSIX_TZ_H
