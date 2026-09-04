//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Linux implementation of shmat
///
//===----------------------------------------------------------------------===//

#include "src/sys/shm/shmat.h"

#include "src/__support/OSUtil/linux/syscall_wrappers/shmat.h"
#include "src/__support/common.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {

// The syscall reports a failure the way the other memory calls do, by
// returning something in the last page rather than a small negative number,
// since an address is what it otherwise gives back.
LLVM_LIBC_FUNCTION(void *, shmat,
                   (int shmid, const void *shmaddr, int shmflg)) {
  ErrorOr<void *> ret = linux_syscalls::shmat(shmid, shmaddr, shmflg);
  if (!ret) {
    libc_errno = ret.error();
    return reinterpret_cast<void *>(-1);
  }
  return *ret;
}

} // namespace LIBC_NAMESPACE_DECL
