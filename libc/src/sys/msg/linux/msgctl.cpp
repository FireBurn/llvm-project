//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Linux implementation of msgctl
///
//===----------------------------------------------------------------------===//

#include "src/sys/msg/msgctl.h"

#include "hdr/types/struct_msqid_ds.h"
#include "src/__support/OSUtil/linux/syscall_wrappers/msgctl.h"
#include "src/__support/common.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {

// The structure the kernel fills in is the wider msqid64_ds, which is what
// struct msqid_ds is here. The syscall asks for that itself rather than
// taking a bit in the command, so the command goes through as it is.
LLVM_LIBC_FUNCTION(int, msgctl, (int msqid, int cmd, struct msqid_ds *buf)) {
  ErrorOr<int> ret = linux_syscalls::msgctl(msqid, cmd, buf);
  if (!ret) {
    libc_errno = ret.error();
    return -1;
  }
  return *ret;
}

} // namespace LIBC_NAMESPACE_DECL
