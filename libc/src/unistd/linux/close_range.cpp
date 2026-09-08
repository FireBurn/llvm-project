//===-- Linux implementation of close_range ------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/unistd/close_range.h"

#include "src/__support/OSUtil/syscall.h" // For internal syscall function.
#include "src/__support/common.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"
#include <sys/syscall.h> // For syscall numbers.

namespace LIBC_NAMESPACE_DECL {

// Closes every descriptor from first to last, which is far cheaper than
// walking /proc to find them.

LLVM_LIBC_FUNCTION(int, close_range,
                   (unsigned int first, unsigned int last, int flags)) {
  int ret =
      LIBC_NAMESPACE::syscall_impl<int>(SYS_close_range, first, last, flags);
  if (ret < 0) {
    libc_errno = -ret;
    return -1;
  }
  return 0;
}

} // namespace LIBC_NAMESPACE_DECL
