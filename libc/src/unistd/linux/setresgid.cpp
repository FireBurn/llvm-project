//===-- Implementation of setresgid ---------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/unistd/setresgid.h"

#include "src/__support/OSUtil/syscall.h"
#include "src/__support/common.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"

#include <sys/syscall.h>

namespace LIBC_NAMESPACE_DECL {

// Any of the three may be -1 to leave that id alone.
LLVM_LIBC_FUNCTION(int, setresgid, (gid_t rgid, gid_t egid, gid_t sgid)) {
  int ret = LIBC_NAMESPACE::syscall_impl<int>(SYS_setresgid, rgid, egid, sgid);
  if (ret < 0) {
    libc_errno = -ret;
    return -1;
  }
  return 0;
}

} // namespace LIBC_NAMESPACE_DECL
