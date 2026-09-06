//===-- Linux implementation of klogctl -----------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/sys/klog/klogctl.h"

#include "src/__support/OSUtil/syscall.h"
#include "src/__support/common.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"

#include <sys/syscall.h>

namespace LIBC_NAMESPACE_DECL {

// Reads and controls the kernel's own log. The system call the kernel gave
// this is called syslog, which is not the logging the C interface of that
// name does, so it goes by this name instead.
LLVM_LIBC_FUNCTION(int, klogctl, (int type, char *bufp, int len)) {
  int ret = LIBC_NAMESPACE::syscall_impl<int>(SYS_syslog, type, bufp, len);
  if (ret < 0) {
    libc_errno = -ret;
    return -1;
  }
  return ret;
}

} // namespace LIBC_NAMESPACE_DECL
