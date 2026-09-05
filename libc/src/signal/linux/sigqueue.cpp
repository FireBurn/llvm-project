//===-- Linux implementation of sigqueue ----------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/signal/sigqueue.h"

#include "hdr/signal_macros.h"
#include "hdr/types/pid_t.h"
#include "hdr/types/siginfo_t.h"
#include "hdr/types/union_sigval.h"
#include "src/__support/OSUtil/syscall.h" // For internal syscall function.
#include "src/__support/common.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"
#include "src/unistd/getpid.h"
#include <sys/syscall.h> // For syscall numbers.

namespace LIBC_NAMESPACE_DECL {

// The value is delivered inside a siginfo_t, which the caller does not
// build: it is put together here so that si_code says the signal came from
// a sigqueue rather than from a kill.
LLVM_LIBC_FUNCTION(int, sigqueue,
                   (pid_t pid, int sig, const union sigval value)) {
  siginfo_t info;
  __builtin_memset(&info, 0, sizeof(info));
  info.si_signo = sig;
  info.si_code = SI_QUEUE;
  info.si_pid = LIBC_NAMESPACE::getpid();
  info.si_uid =
      static_cast<unsigned int>(LIBC_NAMESPACE::syscall_impl<int>(SYS_getuid));
  info.si_value = value;

  int ret =
      LIBC_NAMESPACE::syscall_impl<int>(SYS_rt_sigqueueinfo, pid, sig, &info);
  if (ret < 0) {
    libc_errno = -ret;
    return -1;
  }
  return 0;
}

} // namespace LIBC_NAMESPACE_DECL
