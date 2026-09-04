//===-- Linux implementation of fanotify_mark -----------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/sys/fanotify/fanotify_mark.h"

#include "hdr/stdint_proxy.h"
#include "src/__support/OSUtil/syscall.h" // For internal syscall function.
#include "src/__support/common.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"
#include <sys/syscall.h> // For syscall numbers.

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(int, fanotify_mark,
                   (int fanotify_fd, unsigned int flags, uint64_t mask,
                    int dirfd, const char *pathname)) {
#if defined(SYS_fanotify_mark) && __SIZEOF_LONG__ == 8
  int ret = LIBC_NAMESPACE::syscall_impl<int>(SYS_fanotify_mark, fanotify_fd,
                                              flags, mask, dirfd, pathname);
#else
  // A 32 bit call has to split the mask over two registers, low half first.
  int ret = LIBC_NAMESPACE::syscall_impl<int>(
      SYS_fanotify_mark, fanotify_fd, flags, static_cast<uint32_t>(mask),
      static_cast<uint32_t>(mask >> 32), dirfd, pathname);
#endif
  if (ret < 0) {
    libc_errno = -ret;
    return -1;
  }
  return 0;
}

} // namespace LIBC_NAMESPACE_DECL
