//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Linux implementation of statx
///
//===----------------------------------------------------------------------===//

#include "src/sys/stat/statx.h"

#include "hdr/types/struct_statx.h"
#include "src/__support/OSUtil/linux/syscall_wrappers/statx.h"
#include "src/__support/common.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"
namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(int, statx,
                   (int dirfd, const char *__restrict pathname, int flags,
                    unsigned int mask, struct statx *__restrict statxbuf)) {
  ErrorOr<int> ret =
      linux_syscalls::statx(dirfd, pathname, flags, mask, statxbuf);
  if (!ret) {
    libc_errno = ret.error();
    return -1;
  }
  return 0;
}

} // namespace LIBC_NAMESPACE_DECL
