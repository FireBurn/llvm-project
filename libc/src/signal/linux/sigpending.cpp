//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Linux implementation of sigpending
///
//===----------------------------------------------------------------------===//

#include "src/signal/sigpending.h"

#include "hdr/errno_macros.h"
#include "hdr/types/sigset_t.h"
#include "src/__support/OSUtil/linux/syscall_wrappers/rt_sigpending.h"
#include "src/__support/common.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(int, sigpending, (sigset_t * set)) {
  if (set == nullptr) {
    libc_errno = EFAULT;
    return -1;
  }
  ErrorOr<int> result = linux_syscalls::rt_sigpending(set);
  if (!result) {
    libc_errno = result.error();
    return -1;
  }
  return 0;
}

} // namespace LIBC_NAMESPACE_DECL
