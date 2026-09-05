//===-- Linux implementation of ppoll -------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/poll/ppoll.h"

#include "hdr/types/nfds_t.h"
#include "hdr/types/sigset_t.h"
#include "hdr/types/struct_pollfd.h"
#include "hdr/types/struct_timespec.h"
#include "src/__support/OSUtil/syscall.h" // syscall_impl
#include "src/__support/common.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"

#include <sys/syscall.h> // SYS_ppoll, SYS_ppoll_time64

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(int, ppoll,
                   (struct pollfd * fds, nfds_t nfds,
                    const struct timespec *timeout, const sigset_t *sigmask)) {
  // The kernel wants to be told how wide the signal set is, since its own is
  // narrower than the one the library hands out.
#if defined(SYS_ppoll_time64)
  int ret = LIBC_NAMESPACE::syscall_impl<int>(
      SYS_ppoll_time64, fds, nfds, timeout, sigmask, sizeof(sigset_t));
#elif defined(SYS_ppoll)
  static_assert(
      sizeof(timespec::tv_nsec) == sizeof(long),
      "This syscall is only safe where tv_nsec matches the register size.");
  int ret = LIBC_NAMESPACE::syscall_impl<int>(SYS_ppoll, fds, nfds, timeout,
                                              sigmask, sizeof(sigset_t));
#else
#error "ppoll and ppoll_time64 syscalls not available."
#endif
  if (ret < 0) {
    libc_errno = -ret;
    return -1;
  }
  return ret;
}

} // namespace LIBC_NAMESPACE_DECL
