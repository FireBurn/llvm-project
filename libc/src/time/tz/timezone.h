//===-- The zone the machine keeps time in ----------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC_TIME_TZ_TIMEZONE_H
#define LLVM_LIBC_SRC_TIME_TZ_TIMEZONE_H

#include "hdr/types/struct_tm.h"
#include "hdr/types/time_t.h"
#include "src/__support/error_or.h"
#include "src/__support/macros/config.h"
#include "src/time/tz/posix_tz.h"

namespace LIBC_NAMESPACE_DECL {
namespace tz {

// Reads TZ, or the file it names, and sets tzname, timezone and daylight from
// it. Doing this is what tzset means.
void set_from_environment();

// The same, but only the first time it is asked. Everything that tells the
// local time does this first, as POSIX says it must.
void ensure_loaded();

// What the zone was doing at the given moment, counted from the epoch in UT.
Answer answer_for_time(time_t seconds);

// The same, and restates tzname, timezone and daylight from what the zone was
// going by then. Telling the local time does this, which is what POSIX means
// by saying it behaves as though tzset had been called.
Answer answer_and_publish(time_t seconds);

// Fills in a broken-down time in the local zone, tm_gmtoff, tm_isdst and
// tm_zone included, from a count of seconds from the epoch.
ErrorOr<tm *> to_local(time_t seconds, tm *result);

// Turns a broken-down time stated in the local zone into a count of seconds
// from the epoch, and fills the rest of the struct in from the answer. This is
// what mktime does.
ErrorOr<time_t> from_local_tm(tm *value);

// Turns a moment stated in local time into one counted from the epoch. The
// offset in force is itself a function of the answer, so this settles it by
// trying an offset and looking again.
//
// `daylight_hint` is tm_isdst: past zero it asks for the daylight reading of a
// moment that has two, zero for the standard one, and below zero to let the
// zone decide. `offset` and `name` report the reading it settled on.
time_t from_local(time_t local_seconds, int daylight_hint, int32_t &offset,
                  const char **name, bool &daylight);

} // namespace tz
} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC_TIME_TZ_TIMEZONE_H
