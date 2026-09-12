//===-- Linux implementation of futimes -----------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/sys/time/futimes.h"

#include "hdr/types/struct_timespec.h"
#include "hdr/types/struct_timeval.h"

#include "src/__support/OSUtil/linux/syscall_wrappers/utimensat.h"
#include "src/__support/libc_errno.h"

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(int, futimes, (int fd, const struct timeval times[2])) {
  struct timespec ts[2];
  struct timespec *ts_ptr = nullptr;
  if (times != nullptr) {
    if (times[0].tv_usec < 0 || times[1].tv_usec < 0 ||
        times[0].tv_usec >= 1000000 || times[1].tv_usec >= 1000000) {
      libc_errno = EINVAL;
      return -1;
    }
    for (int i = 0; i < 2; ++i) {
      ts[i].tv_sec = times[i].tv_sec;
      ts[i].tv_nsec =
          static_cast<decltype(ts[i].tv_nsec)>(times[i].tv_usec * 1000);
    }
    ts_ptr = ts;
  }

  // utimensat with no path sets the times of the descriptor itself.
  auto result = linux_syscalls::utimensat(fd, nullptr, ts_ptr, 0);
  if (!result.has_value()) {
    libc_errno = result.error();
    return -1;
  }
  return 0;
}

} // namespace LIBC_NAMESPACE_DECL
