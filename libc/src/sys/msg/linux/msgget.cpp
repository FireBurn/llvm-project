//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Linux implementation of msgget
///
//===----------------------------------------------------------------------===//

#include "src/sys/msg/msgget.h"

#include "hdr/types/key_t.h"
#include "src/__support/OSUtil/linux/syscall_wrappers/msgget.h"
#include "src/__support/common.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(int, msgget, (key_t key, int msgflg)) {
  ErrorOr<int> ret = linux_syscalls::msgget(key, msgflg);
  if (!ret) {
    libc_errno = ret.error();
    return -1;
  }
  return *ret;
}

} // namespace LIBC_NAMESPACE_DECL
