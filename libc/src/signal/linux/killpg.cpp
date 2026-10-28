//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Linux implementation of killpg
///
//===----------------------------------------------------------------------===//

#include "src/signal/killpg.h"

#include "hdr/errno_macros.h"
#include "hdr/types/pid_t.h"
#include "src/__support/OSUtil/linux/syscall_wrappers/kill.h"
#include "src/__support/common.h"
#include "src/__support/error_or.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(int, killpg, (pid_t pgrp, int sig)) {
  if (pgrp < 0) {
    libc_errno = EINVAL;
    return -1;
  }
  // A process group is addressed by negating its id, and a pgrp of zero
  // means the caller's own group, which kill already spells as zero.
  ErrorOr<int> ret = linux_syscalls::kill(-pgrp, sig);
  if (!ret) {
    libc_errno = ret.error();
    return -1;
  }
  return 0;
}

} // namespace LIBC_NAMESPACE_DECL
