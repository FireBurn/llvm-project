//===-- Linux implementation of setpgrp -----------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/unistd/setpgrp.h"

#include "src/__support/OSUtil/linux/syscall_wrappers/setpgid.h"
#include "src/__support/common.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {

// The System V form glibc has: 0 on success, where XSI would return the new
// process group ID.
LLVM_LIBC_FUNCTION(int, setpgrp, ()) {
  auto ret = linux_syscalls::setpgid(0, 0);
  if (!ret) {
    libc_errno = ret.error();
    return -1;
  }
  return 0;
}

} // namespace LIBC_NAMESPACE_DECL
