//===-- Linux implementation of quotactl_fd -------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/sys/quota/quotactl_fd.h"

#include "src/__support/OSUtil/syscall.h" // For internal syscall function.
#include "src/__support/common.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"
#include <sys/syscall.h> // For syscall numbers.

namespace LIBC_NAMESPACE_DECL {

// The same as quotactl, but naming the filesystem by a descriptor open on
// it rather than by the device it is on.
LLVM_LIBC_FUNCTION(int, quotactl_fd, (int fd, int cmd, int id, void *addr)) {
  int ret =
      LIBC_NAMESPACE::syscall_impl<int>(SYS_quotactl_fd, fd, cmd, id, addr);
  if (ret < 0) {
    libc_errno = -ret;
    return -1;
  }
  return 0;
}

} // namespace LIBC_NAMESPACE_DECL
