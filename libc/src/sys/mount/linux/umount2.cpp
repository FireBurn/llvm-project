//===-- Linux implementation of umount2 -----------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/sys/mount/umount2.h"

#include "src/__support/OSUtil/syscall.h"
#include "src/__support/common.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"

#include <sys/syscall.h>

namespace LIBC_NAMESPACE_DECL {

// The syscall Linux calls umount2 is the one named umount2 here as well; the
// kernel's plain umount is an older one which not every architecture has.
LLVM_LIBC_FUNCTION(int, umount2, (const char *target, int flags)) {
  int ret = LIBC_NAMESPACE::syscall_impl<int>(SYS_umount2, target, flags);
  if (ret < 0) {
    libc_errno = -ret;
    return -1;
  }
  return 0;
}

} // namespace LIBC_NAMESPACE_DECL
