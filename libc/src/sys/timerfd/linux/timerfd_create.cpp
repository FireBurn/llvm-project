//===-- Linux implementation of timerfd_create ----------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/sys/timerfd/timerfd_create.h"

#include "src/__support/OSUtil/syscall.h"
#include "src/__support/common.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"

#include <sys/syscall.h>

namespace LIBC_NAMESPACE_DECL {

// A descriptor which becomes readable when a timer fires, so waiting for a
// timer is the same as waiting for anything else. A read hands back how
// many times it has fired since the last one.
LLVM_LIBC_FUNCTION(int, timerfd_create, (int clockid, int flags)) {
  int ret =
      LIBC_NAMESPACE::syscall_impl<int>(SYS_timerfd_create, clockid, flags);
  if (ret < 0) {
    libc_errno = -ret;
    return -1;
  }
  return ret;
}

} // namespace LIBC_NAMESPACE_DECL
