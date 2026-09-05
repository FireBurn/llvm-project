//===-- Linux implementation of unlockpt ----------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/stdlib/unlockpt.h"

#include "hdr/sys_ioctl_macros.h"
#include "src/__support/OSUtil/syscall.h"
#include "src/__support/common.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"
#include <sys/syscall.h>

namespace LIBC_NAMESPACE_DECL {

// Lets the terminal behind the master be opened.
LLVM_LIBC_FUNCTION(int, unlockpt, (int fd)) {
  int unlock = 0;
  int ret =
      LIBC_NAMESPACE::syscall_impl<int>(SYS_ioctl, fd, TIOCSPTLCK, &unlock);
  if (ret < 0) {
    libc_errno = -ret;
    return -1;
  }
  return 0;
}

} // namespace LIBC_NAMESPACE_DECL
