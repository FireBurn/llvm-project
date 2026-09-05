//===-- Linux implementation of pause ------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/unistd/pause.h"
#include "hdr/errno_macros.h"
#include "src/__support/threads/cancel.h"

#include "src/__support/OSUtil/syscall.h" // For internal syscall function.
#include "src/__support/common.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"
#include <sys/syscall.h> // For syscall numbers.

namespace LIBC_NAMESPACE_DECL {

// Waits for a signal and nothing else. It only ever returns -1, with errno
// set to EINTR, once a handler has run.
LLVM_LIBC_FUNCTION(int, pause, (void)) {
  internal::cancel_point();
#ifdef SYS_pause
  int ret = LIBC_NAMESPACE::syscall_impl<int>(SYS_pause);
#else
  // Where there is no pause, waiting on nothing with no deadline is the
  // same thing.
  int ret = LIBC_NAMESPACE::syscall_impl<int>(SYS_ppoll, nullptr, 0, nullptr,
                                              nullptr, 0);
#endif
  if (ret == -EINTR)
    internal::cancel_point();

  if (ret < 0) {
    libc_errno = -ret;
    return -1;
  }
  return ret;
}

} // namespace LIBC_NAMESPACE_DECL
