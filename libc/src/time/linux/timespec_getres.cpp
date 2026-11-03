//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation of timespec_getres.
///
//===----------------------------------------------------------------------===//

#include "src/time/timespec_getres.h"
#include "hdr/time_macros.h"
#include "hdr/types/clockid_t.h"
#include "hdr/types/struct_timespec.h"
#include "src/__support/OSUtil/linux/syscall_wrappers/clock_getres.h"
#include "src/__support/common.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {

// Reports the resolution of the clock timespec_get reads for the same base,
// and returns the base, or zero if the base is not supported.
LLVM_LIBC_FUNCTION(int, timespec_getres, (timespec * res, int base)) {
  clockid_t clockid;
  switch (base) {
  case TIME_UTC:
    clockid = CLOCK_REALTIME;
    break;
  case TIME_MONOTONIC:
    clockid = CLOCK_MONOTONIC;
    break;
  case TIME_ACTIVE:
    clockid = CLOCK_PROCESS_CPUTIME_ID;
    break;
  case TIME_THREAD_ACTIVE:
    clockid = CLOCK_THREAD_CPUTIME_ID;
    break;
  default:
    return 0;
  }

  // The result may be left out, to ask only whether the base is supported.
  if (res == nullptr)
    return base;

  auto result = linux_syscalls::clock_getres(clockid, res);
  if (!result.has_value()) {
    libc_errno = result.error();
    return 0;
  }
  return base;
}

} // namespace LIBC_NAMESPACE_DECL
