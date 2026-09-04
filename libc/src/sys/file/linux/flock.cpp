//===-- Linux implementation of flock -------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/sys/file/flock.h"

#include "src/__support/OSUtil/syscall.h"
#include "src/__support/common.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"

#include <sys/syscall.h>

namespace LIBC_NAMESPACE_DECL {

// The lock flock places is on the open file description, so it is shared by
// every descriptor duplicated from it and released when the last of them is
// closed. It is a different thing from the locks fcntl places.
LLVM_LIBC_FUNCTION(int, flock, (int fd, int operation)) {
  int ret = LIBC_NAMESPACE::syscall_impl<int>(SYS_flock, fd, operation);
  if (ret < 0) {
    libc_errno = -ret;
    return -1;
  }
  return 0;
}

} // namespace LIBC_NAMESPACE_DECL
