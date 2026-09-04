//===-- Implementation of sigismember -------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/signal/sigismember.h"

#include "hdr/types/sigset_t.h"
#include "src/__support/common.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"
#include "src/signal/linux/signal_utils.h"

namespace LIBC_NAMESPACE_DECL {

// Returns 1 when the signal is in the set and 0 when it is not. An invalid
// signal number is an error rather than an absence, so it gives -1.
LLVM_LIBC_FUNCTION(int, sigismember, (const sigset_t *set, int signum)) {
  if (set == nullptr || signum <= 0 || signum >= NSIG) {
    libc_errno = EINVAL;
    return -1;
  }
  return has_signal(*set, signum) ? 1 : 0;
}

} // namespace LIBC_NAMESPACE_DECL
