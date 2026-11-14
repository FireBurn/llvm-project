//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Linux implementation of timer_create function.
///
//===----------------------------------------------------------------------===//

#include "src/time/timer_create.h"
#include "hdr/errno_macros.h"
#include "hdr/signal_macros.h"
#include "src/__support/OSUtil/linux/syscall_wrappers/timer_create.h"
#include "src/__support/common.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"
#include "src/time/linux/sigev_thread.h"

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(int, timer_create,
                   (clockid_t clockid, sigevent *__restrict sevp,
                    timer_t *__restrict timerid)) {
  if (sevp != nullptr && sevp->sigev_notify == SIGEV_THREAD) {
    if (timerid == nullptr) {
      libc_errno = EFAULT;
      return -1;
    }
    int error = internal::sigev_thread::create(clockid, sevp, timerid);
    if (error != 0) {
      libc_errno = error;
      return -1;
    }
    return 0;
  }
  auto result = linux_syscalls::timer_create(clockid, sevp, timerid);
  if (!result) {
    libc_errno = result.error();
    return -1;
  }
  return result.value();
}

} // namespace LIBC_NAMESPACE_DECL
