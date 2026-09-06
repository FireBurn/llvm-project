//===-- Linux implementation of vhangup -----------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/unistd/vhangup.h"

#include "src/__support/OSUtil/syscall.h"
#include "src/__support/common.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"

#include <sys/syscall.h>

namespace LIBC_NAMESPACE_DECL {

// Hangs up the terminal the caller is on, so that whatever was reading it
// sees the end of its input. A login program does this before handing the
// terminal to the next user.
LLVM_LIBC_FUNCTION(int, vhangup, (void)) {
  int ret = LIBC_NAMESPACE::syscall_impl<int>(SYS_vhangup);
  if (ret < 0) {
    libc_errno = -ret;
    return -1;
  }
  return 0;
}

} // namespace LIBC_NAMESPACE_DECL
