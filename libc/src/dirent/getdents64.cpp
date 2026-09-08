//===-- Linux implementation of getdents64 -------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/dirent/getdents64.h"

#include "hdr/types/size_t.h"
#include "hdr/types/ssize_t.h"
#include "src/__support/OSUtil/syscall.h" // For internal syscall function.
#include "src/__support/common.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"
#include <sys/syscall.h> // For syscall numbers.

namespace LIBC_NAMESPACE_DECL {

// Reads directory entries straight from the kernel, in the layout it
// writes them, rather than one struct dirent at a time.

LLVM_LIBC_FUNCTION(ssize_t, getdents64, (int fd, void *buf, size_t count)) {
  ssize_t ret =
      LIBC_NAMESPACE::syscall_impl<ssize_t>(SYS_getdents64, fd, buf, count);
  if (ret < 0) {
    libc_errno = static_cast<int>(-ret);
    return -1;
  }
  return ret;
}

} // namespace LIBC_NAMESPACE_DECL
