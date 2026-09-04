//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Linux implementation of msgsnd
///
//===----------------------------------------------------------------------===//

#include "src/sys/msg/msgsnd.h"

#include "hdr/types/size_t.h"
#include "src/__support/OSUtil/linux/syscall_wrappers/msgsnd.h"
#include "src/__support/common.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(int, msgsnd,
                   (int msqid, const void *msgp, size_t msgsz, int msgflg)) {
  ErrorOr<int> ret = linux_syscalls::msgsnd(msqid, msgp, msgsz, msgflg);
  if (!ret) {
    libc_errno = ret.error();
    return -1;
  }
  return 0;
}

} // namespace LIBC_NAMESPACE_DECL
