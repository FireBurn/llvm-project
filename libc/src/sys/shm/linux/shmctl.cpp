//===-- Linux implementation of shmctl ------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/sys/shm/shmctl.h"

#include "hdr/types/struct_shmid_ds.h"
#include "src/__support/OSUtil/syscall.h"
#include "src/__support/common.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"

#include <sys/syscall.h>

namespace LIBC_NAMESPACE_DECL {

// The structure the kernel fills in is the wider shmid64_ds, which is what
// struct shmid_ds is here, and the syscall asks for that itself, so the
// command goes through as it is.
LLVM_LIBC_FUNCTION(int, shmctl, (int shmid, int cmd, struct shmid_ds *buf)) {
  int ret = LIBC_NAMESPACE::syscall_impl<int>(SYS_shmctl, shmid, cmd, buf);
  if (ret < 0) {
    libc_errno = -ret;
    return -1;
  }
  return ret;
}

} // namespace LIBC_NAMESPACE_DECL
