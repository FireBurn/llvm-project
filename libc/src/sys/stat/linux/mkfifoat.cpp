//===-- Linux implementation of mkfifoat ----------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/sys/stat/mkfifoat.h"

#include "hdr/sys_stat_macros.h"
#include "hdr/types/mode_t.h"
#include "src/__support/OSUtil/syscall.h" // For internal syscall function.
#include "src/__support/common.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"
#include <sys/syscall.h> // For syscall numbers.

namespace LIBC_NAMESPACE_DECL {

// A fifo is a node of the fifo kind, which is all mknodat needs to be told.
LLVM_LIBC_FUNCTION(int, mkfifoat, (int dirfd, const char *path, mode_t mode)) {
  int ret = LIBC_NAMESPACE::syscall_impl<int>(SYS_mknodat, dirfd, path,
                                              mode | S_IFIFO, 0);
  if (ret < 0) {
    libc_errno = -ret;
    return -1;
  }
  return 0;
}

} // namespace LIBC_NAMESPACE_DECL
