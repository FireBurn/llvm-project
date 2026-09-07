//===-- Linux implementation of tee ---------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/fcntl/tee.h"

#include "src/__support/OSUtil/syscall.h" // For internal syscall function.
#include "src/__support/common.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"
#include <sys/syscall.h> // For syscall numbers.

namespace LIBC_NAMESPACE_DECL {

// Copies what one pipe holds into another without taking it out of the first,
// so that the same bytes can be read twice.
LLVM_LIBC_FUNCTION(ssize_t, tee,
                   (int fd_in, int fd_out, size_t len, unsigned int flags)) {
  ssize_t ret =
      LIBC_NAMESPACE::syscall_impl<ssize_t>(SYS_tee, fd_in, fd_out, len, flags);
  if (ret < 0) {
    libc_errno = static_cast<int>(-ret);
    return -1;
  }
  return ret;
}

} // namespace LIBC_NAMESPACE_DECL
