//===-- Linux implementation of fchownat ----------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/unistd/fchownat.h"

#include "src/__support/OSUtil/syscall.h" // For internal syscall function.
#include "src/__support/common.h"

#include "hdr/types/gid_t.h"
#include "hdr/types/uid_t.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"
#include <sys/syscall.h> // For syscall numbers.

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(int, fchownat,
                   (int dirfd, const char *path, uid_t owner, gid_t group,
                    int flags)) {
  int ret = LIBC_NAMESPACE::syscall_impl<int>(SYS_fchownat, dirfd, path, owner,
                                              group, flags);
  if (ret < 0) {
    libc_errno = -ret;
    return -1;
  }
  return 0;
}

} // namespace LIBC_NAMESPACE_DECL
