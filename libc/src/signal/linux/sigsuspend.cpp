//===-- Linux implementation of sigsuspend --------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/signal/sigsuspend.h"

#include "hdr/errno_macros.h"
#include "hdr/types/sigset_t.h"
#include "src/__support/OSUtil/linux/syscall_wrappers/rt_sigsuspend.h"
#include "src/__support/common.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"
#include "src/__support/threads/cancel.h"

namespace LIBC_NAMESPACE_DECL {

// sigsuspend only ever returns after a handler has run, so it always fails,
// with EINTR when it did what was asked of it.
LLVM_LIBC_FUNCTION(int, sigsuspend, (const sigset_t *set)) {
  if (set == nullptr) {
    libc_errno = EINVAL;
    return -1;
  }
  auto result =
      internal::cancellable([&] { return linux_syscalls::rt_sigsuspend(set); });
  libc_errno = result.has_value() ? EINTR : result.error();
  return -1;
}

} // namespace LIBC_NAMESPACE_DECL
