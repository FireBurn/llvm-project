//===-- Linux implementation of lockf -------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/unistd/lockf.h"
#include "hdr/fcntl_macros.h"
#include "hdr/stdio_macros.h" // For SEEK_CUR
#include "hdr/types/off_t.h"
#include "hdr/types/struct_flock.h"
#include "src/__support/common.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"
#include "src/fcntl/fcntl.h"

namespace LIBC_NAMESPACE_DECL {

// lockf is the older, narrower way to ask for what fcntl's record locks
// already do: a write lock, on a region which starts where the file offset
// is, and belongs to the process rather than the descriptor.
LLVM_LIBC_FUNCTION(int, lockf, (int fd, int cmd, off_t len)) {
  struct flock lock;
  lock.l_whence = SEEK_CUR;
  lock.l_start = 0;
  lock.l_len = len;
  lock.l_type = F_WRLCK;

  switch (cmd) {
  case F_ULOCK:
    lock.l_type = F_UNLCK;
    return fcntl(fd, F_SETLK, &lock);
  case F_LOCK:
    return fcntl(fd, F_SETLKW, &lock);
  case F_TLOCK:
    return fcntl(fd, F_SETLK, &lock);
  case F_TEST:
    // A test reports whether the region could be locked, and says nothing
    // about a lock this process already holds: F_GETLK answers F_UNLCK for
    // one of its own.
    if (fcntl(fd, F_GETLK, &lock) < 0)
      return -1;
    if (lock.l_type == F_UNLCK)
      return 0;
    libc_errno = EACCES;
    return -1;
  default:
    libc_errno = EINVAL;
    return -1;
  }
}

} // namespace LIBC_NAMESPACE_DECL
