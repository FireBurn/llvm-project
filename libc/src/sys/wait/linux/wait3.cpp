//===-- Linux implementation of wait3 -------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Linux implementation of wait3.
///
//===----------------------------------------------------------------------===//

#include "src/sys/wait/wait3.h"

#include "src/__support/OSUtil/linux/syscall_wrappers/wait4.h"
#include "src/__support/common.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(pid_t, wait3,
                   (int *wait_status, int options, struct rusage *usage)) {
  auto result = linux_syscalls::wait4(-1, wait_status, options, usage);
  if (!result.has_value()) {
    libc_errno = result.error();
    return -1;
  }
  return result.value();
}

} // namespace LIBC_NAMESPACE_DECL
