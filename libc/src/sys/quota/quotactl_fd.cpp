//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Linux implementation of quotactl_fd
///
//===----------------------------------------------------------------------===//

#include "src/sys/quota/quotactl_fd.h"

#include "src/__support/OSUtil/linux/syscall_wrappers/quotactl_fd.h"
#include "src/__support/common.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"
namespace LIBC_NAMESPACE_DECL {

// The same as quotactl, but naming the filesystem by a descriptor open on
// it rather than by the device it is on.
LLVM_LIBC_FUNCTION(int, quotactl_fd, (int fd, int cmd, int id, void *addr)) {
  ErrorOr<int> ret = linux_syscalls::quotactl_fd(fd, cmd, id, addr);
  if (!ret) {
    libc_errno = ret.error();
    return -1;
  }
  return 0;
}

} // namespace LIBC_NAMESPACE_DECL
