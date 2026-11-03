//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation of mseal.
///
//===----------------------------------------------------------------------===//

#include "src/__support/OSUtil/linux/syscall_wrappers/mseal.h"
#include "hdr/types/size_t.h"
#include "src/__support/common.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"
#include "src/sys/mman/mseal.h"

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(int, mseal, (void *addr, size_t len, unsigned long flags)) {
  ErrorOr<int> ret = linux_syscalls::mseal(addr, len, flags);
  if (!ret) {
    libc_errno = ret.error();
    return -1;
  }
  return ret.value();
}

} // namespace LIBC_NAMESPACE_DECL
