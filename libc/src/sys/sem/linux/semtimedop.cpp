//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Linux implementation of semtimedop
///
//===----------------------------------------------------------------------===//

#include "src/sys/sem/semtimedop.h"

#include "src/__support/OSUtil/linux/syscall_wrappers/semtimedop.h"
#include "src/__support/common.h"
#include "src/__support/libc_errno.h"
namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(int, semtimedop,
                   (int semid, struct sembuf *sops, size_t nsops,
                    const struct timespec *timeout)) {
  ErrorOr<int> ret = linux_syscalls::semtimedop(semid, sops, nsops, timeout);
  if (!ret) {
    libc_errno = ret.error();
    return -1;
  }
  return *ret;
}

} // namespace LIBC_NAMESPACE_DECL
