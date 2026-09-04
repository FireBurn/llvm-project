//===-- Linux implementation of shmat -------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/sys/shm/shmat.h"

#include "src/__support/OSUtil/syscall.h"
#include "src/__support/common.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"

#include <sys/syscall.h>

namespace LIBC_NAMESPACE_DECL {

// The syscall reports a failure the way the other memory calls do, by
// returning something in the last page rather than a small negative number,
// since an address is what it otherwise gives back.
LLVM_LIBC_FUNCTION(void *, shmat,
                   (int shmid, const void *shmaddr, int shmflg)) {
  long ret =
      LIBC_NAMESPACE::syscall_impl<long>(SYS_shmat, shmid, shmaddr, shmflg);
  if (ret < 0 && ret > -4096) {
    libc_errno = static_cast<int>(-ret);
    return reinterpret_cast<void *>(-1);
  }
  return reinterpret_cast<void *>(ret);
}

} // namespace LIBC_NAMESPACE_DECL
