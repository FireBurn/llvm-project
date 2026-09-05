//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Linux implementation of clock_adjtime
///
//===----------------------------------------------------------------------===//

#include "src/sys/timex/clock_adjtime.h"

#include "hdr/types/clockid_t.h"
#include "hdr/types/struct_timex.h"
#include "src/__support/OSUtil/linux/syscall_wrappers/clock_adjtime.h"
#include "src/__support/common.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"
namespace LIBC_NAMESPACE_DECL {

// adjtimex for a named clock rather than always the system one, which is
// what steers a clock a device provides.
LLVM_LIBC_FUNCTION(int, clock_adjtime, (clockid_t clk_id, struct timex *buf)) {
  ErrorOr<int> ret = linux_syscalls::clock_adjtime(clk_id, buf);
  if (!ret) {
    libc_errno = ret.error();
    return -1;
  }
  return *ret;
}

} // namespace LIBC_NAMESPACE_DECL
