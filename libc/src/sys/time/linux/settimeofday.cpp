//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Linux implementation of settimeofday
///
//===----------------------------------------------------------------------===//

#include "src/sys/time/settimeofday.h"

#include "hdr/errno_macros.h"
#include "hdr/time_macros.h"
#include "hdr/types/struct_timespec.h"
#include "hdr/types/struct_timeval.h"
#include "hdr/types/struct_timezone.h"
#include "src/__support/common.h"
#include "src/__support/error_or.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"
#include "src/__support/time/clock_settime.h"
#include "src/__support/time/units.h"

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(int, settimeofday,
                   (const struct timeval *tv, const struct timezone *tz)) {
  // The timezone argument has been unused since the kernel stopped keeping
  // one, and passing anything but a null pointer is an error.
  if (tz != nullptr) {
    libc_errno = EINVAL;
    return -1;
  }
  if (tv == nullptr)
    return 0;

  using namespace time_units;
  // A microsecond count of a second or more is not a time of day.
  if (tv->tv_usec < 0 || tv->tv_usec >= 1_s_us) {
    libc_errno = EINVAL;
    return -1;
  }

  struct timespec ts;
  ts.tv_sec = tv->tv_sec;
  ts.tv_nsec = static_cast<long>(tv->tv_usec) * 1_us_ns;
  ErrorOr<int> ret = internal::clock_settime(CLOCK_REALTIME, &ts);
  if (!ret) {
    libc_errno = ret.error();
    return -1;
  }
  return 0;
}

} // namespace LIBC_NAMESPACE_DECL
