//===-- Linux implementation of times -------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/sys/times/times.h"

#include "hdr/types/clock_t.h"
#include "hdr/types/struct_tms.h"
#include "src/__support/OSUtil/syscall.h"
#include "src/__support/common.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"

#include <sys/syscall.h>

namespace LIBC_NAMESPACE_DECL {

// What comes back is a count of clock ticks since some point in the past that
// is the same for every call, so only the difference between two of them
// means anything. A tick count is allowed to be negative, so a caller that
// wants to know whether this failed has to clear errno first and look at it
// afterwards rather than compare the result.
LLVM_LIBC_FUNCTION(clock_t, times, (struct tms * buffer)) {
  const long ret = LIBC_NAMESPACE::syscall_impl<long>(SYS_times, buffer);
  if (ret < 0 && ret > -4096) {
    libc_errno = static_cast<int>(-ret);
    return static_cast<clock_t>(-1);
  }
  return static_cast<clock_t>(ret);
}

} // namespace LIBC_NAMESPACE_DECL
