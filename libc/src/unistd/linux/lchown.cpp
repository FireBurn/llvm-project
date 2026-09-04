//===-- Linux implementation of lchown ------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/unistd/lchown.h"

#include "hdr/fcntl_macros.h"
#include "hdr/types/gid_t.h"
#include "hdr/types/uid_t.h"
#include "src/__support/OSUtil/syscall.h"
#include "src/__support/common.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"

#include <sys/syscall.h> // For syscall numbers.

namespace LIBC_NAMESPACE_DECL {

// lchown is chown which does not follow a symbolic link, so it changes the
// owner of the link itself rather than of what it points at.
LLVM_LIBC_FUNCTION(int, lchown, (const char *path, uid_t owner, gid_t group)) {
#ifdef SYS_lchown
  int ret = LIBC_NAMESPACE::syscall_impl<int>(SYS_lchown, path, owner, group);
#elif defined(SYS_fchownat)
  int ret = LIBC_NAMESPACE::syscall_impl<int>(
      SYS_fchownat, AT_FDCWD, path, owner, group, AT_SYMLINK_NOFOLLOW);
#else
#error "lchown and fchownat syscalls not available."
#endif

  if (ret < 0) {
    libc_errno = -ret;
    return -1;
  }
  return 0;
}

} // namespace LIBC_NAMESPACE_DECL
