//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Linux implementation of vhangup
///
//===----------------------------------------------------------------------===//

#include "src/unistd/vhangup.h"

#include "src/__support/OSUtil/linux/syscall_wrappers/vhangup.h"
#include "src/__support/common.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {

// Hangs up the terminal the caller is on, so that whatever was reading it
// sees the end of its input. A login program does this before handing the
// terminal to the next user.
LLVM_LIBC_FUNCTION(int, vhangup, (void)) {
  ErrorOr<int> ret = linux_syscalls::vhangup();
  if (!ret) {
    libc_errno = ret.error();
    return -1;
  }
  return 0;
}

} // namespace LIBC_NAMESPACE_DECL
