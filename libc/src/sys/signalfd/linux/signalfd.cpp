//===-- Linux implementation of signalfd ----------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/sys/signalfd/signalfd.h"

#include "hdr/types/sigset_t.h"
#include "src/__support/OSUtil/syscall.h"
#include "src/__support/common.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"

#include <sys/syscall.h>

namespace LIBC_NAMESPACE_DECL {

// A descriptor which becomes readable when one of the signals in the mask
// arrives, so a program can wait for a signal in the same place it waits
// for everything else instead of in a handler.
LLVM_LIBC_FUNCTION(int, signalfd, (int fd, const sigset_t *mask, int flags)) {
#ifdef SYS_signalfd4
  int ret = LIBC_NAMESPACE::syscall_impl<int>(SYS_signalfd4, fd, mask,
                                              sizeof(sigset_t), flags);
#elif defined(SYS_signalfd)
  if (flags != 0) {
    libc_errno = EINVAL;
    return -1;
  }
  int ret = LIBC_NAMESPACE::syscall_impl<int>(SYS_signalfd, fd, mask,
                                              sizeof(sigset_t));
#else
#error "signalfd and signalfd4 syscalls not available."
#endif
  if (ret < 0) {
    libc_errno = -ret;
    return -1;
  }
  return ret;
}

} // namespace LIBC_NAMESPACE_DECL
