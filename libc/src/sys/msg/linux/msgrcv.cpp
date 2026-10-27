//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Linux implementation of msgrcv
///
//===----------------------------------------------------------------------===//

#include "src/sys/msg/msgrcv.h"

#include "hdr/types/size_t.h"
#include "hdr/types/ssize_t.h"
#include "src/__support/OSUtil/linux/syscall_wrappers/msgrcv.h"
#include "src/__support/common.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(ssize_t, msgrcv,
                   (int msqid, void *msgp, size_t msgsz, long msgtyp,
                    int msgflg)) {
  ErrorOr<long> ret =
      linux_syscalls::msgrcv(msqid, msgp, msgsz, msgtyp, msgflg);
  if (!ret) {
    libc_errno = ret.error();
    return -1;
  }
  return static_cast<ssize_t>(*ret);
}

} // namespace LIBC_NAMESPACE_DECL
