//===-- Linux implementation of pselect -----------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/sys/select/pselect.h"

#include "hdr/types/struct_timespec.h"
#include "src/__support/common.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"
#include "src/sys/select/linux/select_utils.h"

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(int, pselect,
                   (int nfds, fd_set *__restrict read_set,
                    fd_set *__restrict write_set, fd_set *__restrict error_set,
                    const struct timespec *__restrict timeout,
                    const sigset_t *__restrict sigmask)) {
  // The syscall writes the remaining time back, which pselect must not do to
  // the caller's timespec, so it gets a copy.
  struct timespec ts;
  struct timespec *tsp = nullptr;
  if (timeout != nullptr) {
    ts = *timeout;
    tsp = &ts;
  }
  internal::pselect6_sigset_t pss{sigmask, sizeof(sigset_t)};
  int ret = internal::pselect6(nfds, read_set, write_set, error_set, tsp, &pss);
  if (ret < 0) {
    libc_errno = -ret;
    return -1;
  }
  return ret;
}

} // namespace LIBC_NAMESPACE_DECL
