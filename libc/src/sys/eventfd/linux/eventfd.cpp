//===-- Linux implementation of eventfd -----------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/sys/eventfd/eventfd.h"

#include "src/__support/OSUtil/syscall.h"
#include "src/__support/common.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"

#include <sys/syscall.h>

namespace LIBC_NAMESPACE_DECL {

// A descriptor holding a counter, which reading takes from and writing adds
// to, and which is readable whenever the count is not zero.
LLVM_LIBC_FUNCTION(int, eventfd, (unsigned int initval, int flags)) {
#ifdef SYS_eventfd2
  int ret = LIBC_NAMESPACE::syscall_impl<int>(SYS_eventfd2, initval, flags);
#elif defined(SYS_eventfd)
  // The older call takes no flags, so a caller asking for any is asking for
  // something it cannot do.
  if (flags != 0) {
    libc_errno = EINVAL;
    return -1;
  }
  int ret = LIBC_NAMESPACE::syscall_impl<int>(SYS_eventfd, initval);
#else
#error "eventfd and eventfd2 syscalls not available."
#endif
  if (ret < 0) {
    libc_errno = -ret;
    return -1;
  }
  return ret;
}

} // namespace LIBC_NAMESPACE_DECL
