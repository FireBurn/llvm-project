//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Linux implementation of clock_nanosleep function.
///
//===----------------------------------------------------------------------===//

#include "src/time/clock_nanosleep.h"
#include "hdr/errno_macros.h"
#include "hdr/time_macros.h"
#include "hdr/types/clockid_t.h"
#include "hdr/types/struct_timespec.h"
#include "src/__support/OSUtil/linux/syscall_wrappers/clock_nanosleep.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(int, clock_nanosleep,
                   (clockid_t clockid, int flags, const timespec *req,
                    timespec *rem)) {
  // The kernel reads any set bit as TIMER_ABSTIME, so other flags would be
  // taken for it rather than refused.
  if (flags & ~TIMER_ABSTIME)
    return EINVAL;
  auto result = linux_syscalls::clock_nanosleep(clockid, flags, req, rem);
  if (!result.has_value())
    return result.error();
  return 0;
}

} // namespace LIBC_NAMESPACE_DECL
