//===-- Linux implementation of fallocate ---------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/fcntl/fallocate.h"

#include "hdr/types/off_t.h"
#include "src/__support/OSUtil/syscall.h" // For internal syscall function.
#include "src/__support/common.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"
#include <sys/syscall.h> // For syscall numbers.

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(int, fallocate,
                   (int fd, int mode, off_t offset, off_t len)) {
#if __SIZEOF_LONG__ == 8
  int ret =
      LIBC_NAMESPACE::syscall_impl<int>(SYS_fallocate, fd, mode, offset, len);
#else
  // A 32 bit call takes each offset as a pair of registers, low half first.
  int ret = LIBC_NAMESPACE::syscall_impl<int>(
      SYS_fallocate, fd, mode, static_cast<long>(offset),
      static_cast<long>(offset >> 32), static_cast<long>(len),
      static_cast<long>(len >> 32));
#endif
  if (ret < 0) {
    libc_errno = -ret;
    return -1;
  }
  return 0;
}

} // namespace LIBC_NAMESPACE_DECL
