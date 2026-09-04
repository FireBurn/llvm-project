//===-- Linux implementation of timerfd_gettime ---------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/sys/timerfd/timerfd_gettime.h"

#include "hdr/types/struct_itimerspec.h"
#include "src/__support/OSUtil/syscall.h"
#include "src/__support/common.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"

#include <sys/syscall.h>

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(int, timerfd_gettime,
                   (int fd, struct itimerspec *curr_value)) {
#ifdef SYS_timerfd_gettime64
  int ret =
      LIBC_NAMESPACE::syscall_impl<int>(SYS_timerfd_gettime64, fd, curr_value);
#elif defined(SYS_timerfd_gettime)
  static_assert(
      sizeof(timespec::tv_nsec) == sizeof(long),
      "This legacy syscall fallback is only safe on platforms where tv_nsec "
      "matches the register size (long). It is unsafe on 32-bit platforms "
      "with 64-bit tv_nsec.");
  int ret =
      LIBC_NAMESPACE::syscall_impl<int>(SYS_timerfd_gettime, fd, curr_value);
#else
#error "timerfd_gettime syscalls not available."
#endif
  if (ret < 0) {
    libc_errno = -ret;
    return -1;
  }
  return 0;
}

} // namespace LIBC_NAMESPACE_DECL
