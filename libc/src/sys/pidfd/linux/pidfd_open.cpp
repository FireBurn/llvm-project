//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation of pidfd_open.
///
//===----------------------------------------------------------------------===//

#include "src/__support/OSUtil/linux/syscall_wrappers/pidfd_open.h"
#include "hdr/types/pid_t.h"
#include "src/__support/common.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"
#include "src/sys/pidfd/pidfd_open.h"

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(int, pidfd_open, (pid_t pid, unsigned int flags)) {
  ErrorOr<int> ret = linux_syscalls::pidfd_open(pid, flags);
  if (!ret) {
    libc_errno = ret.error();
    return -1;
  }
  return ret.value();
}

} // namespace LIBC_NAMESPACE_DECL
