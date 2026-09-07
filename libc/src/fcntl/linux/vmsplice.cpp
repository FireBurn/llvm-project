//===-- Linux implementation of vmsplice ----------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/fcntl/vmsplice.h"

#include "src/__support/OSUtil/syscall.h" // For internal syscall function.
#include "src/__support/common.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"
#include <sys/syscall.h> // For syscall numbers.

namespace LIBC_NAMESPACE_DECL {

// Moves what the caller's own memory holds into a pipe, or takes what a pipe
// holds into it, without a copy: splice does the same between two
// descriptors, and this is the end of it that is not one.
LLVM_LIBC_FUNCTION(ssize_t, vmsplice,
                   (int fd, const struct iovec *iov, size_t count,
                    unsigned int flags)) {
  ssize_t ret = LIBC_NAMESPACE::syscall_impl<ssize_t>(SYS_vmsplice, fd, iov,
                                                      count, flags);
  if (ret < 0) {
    libc_errno = static_cast<int>(-ret);
    return -1;
  }
  return ret;
}

} // namespace LIBC_NAMESPACE_DECL
