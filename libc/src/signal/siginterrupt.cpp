//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation of siginterrupt
///
//===----------------------------------------------------------------------===//

#include "src/signal/siginterrupt.h"

#include "hdr/signal_macros.h"
#include "hdr/types/struct_sigaction.h"
#include "src/__support/common.h"
#include "src/__support/error_or.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"
#include "src/signal/linux/signal_utils.h"

namespace LIBC_NAMESPACE_DECL {

// This says whether a signal arriving during a slow system call makes the
// call fail with EINTR or start again, which is the one thing SA_RESTART
// decides. It is stated here as a change to what the signal is already set
// to, since that is all it has ever been.
LLVM_LIBC_FUNCTION(int, siginterrupt, (int signal, int flag)) {
  struct sigaction action;
  if (ErrorOr<int> ret = checked_sigaction(signal, nullptr, &action); !ret) {
    libc_errno = ret.error();
    return -1;
  }

  if (flag != 0)
    action.sa_flags &= ~SA_RESTART;
  else
    action.sa_flags |= SA_RESTART;

  if (ErrorOr<int> ret = checked_sigaction(signal, &action, nullptr); !ret) {
    libc_errno = ret.error();
    return -1;
  }
  return 0;
}

} // namespace LIBC_NAMESPACE_DECL
