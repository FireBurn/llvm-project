//===-- Linux implementation of sigwait -----------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/signal/sigwait.h"

#include "hdr/errno_macros.h"
#include "hdr/types/siginfo_t.h"
#include "src/__support/common.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"
#include "src/signal/sigwaitinfo.h"

namespace LIBC_NAMESPACE_DECL {

// The same wait as sigwaitinfo, reporting only which signal arrived and
// reporting what went wrong as its result rather than through errno.
LLVM_LIBC_FUNCTION(int, sigwait,
                   (const sigset_t *__restrict set, int *__restrict sig)) {
  siginfo_t info;
  const int saved = libc_errno;
  libc_errno = 0;
  const int signal = LIBC_NAMESPACE::sigwaitinfo(set, &info);
  if (signal < 0) {
    const int reason = libc_errno;
    libc_errno = saved;
    return reason;
  }
  libc_errno = saved;
  if (sig != nullptr)
    *sig = info.si_signo;
  return 0;
}

} // namespace LIBC_NAMESPACE_DECL
