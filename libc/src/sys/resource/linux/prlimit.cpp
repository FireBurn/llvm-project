//===-- Linux implementation of prlimit ----------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/sys/resource/prlimit.h"

#include "hdr/types/pid_t.h"
#include "hdr/types/struct_rlimit.h"
#include "src/__support/OSUtil/syscall.h" // For internal syscall function.
#include "src/__support/common.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"
#include <sys/syscall.h> // For syscall numbers.

namespace LIBC_NAMESPACE_DECL {

// Reads or changes a limit of another process, not only of this one.

LLVM_LIBC_FUNCTION(int, prlimit,
                   (pid_t pid, int resource, const struct rlimit *new_limit,
                    struct rlimit *old_limit)) {
  int ret = LIBC_NAMESPACE::syscall_impl<int>(SYS_prlimit64, pid, resource,
                                              new_limit, old_limit);
  if (ret < 0) {
    libc_errno = -ret;
    return -1;
  }
  return 0;
}

} // namespace LIBC_NAMESPACE_DECL
