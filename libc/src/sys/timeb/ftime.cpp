//===-- Implementation of ftime -------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/sys/timeb/ftime.h"
#include "hdr/time_macros.h"
#include "hdr/types/struct_timespec.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/__support/time/clock_gettime.h"

namespace LIBC_NAMESPACE_DECL {

// The time zone fields have been unused for a long time; glibc and musl both
// leave them zero.
LLVM_LIBC_FUNCTION(int, ftime, (struct timeb * tp)) {
  timespec ts{};
  internal::clock_gettime(CLOCK_REALTIME, &ts);
  tp->time = ts.tv_sec;
  tp->millitm = static_cast<unsigned short>(ts.tv_nsec / 1000000);
  tp->timezone = 0;
  tp->dstflag = 0;
  return 0;
}

} // namespace LIBC_NAMESPACE_DECL
