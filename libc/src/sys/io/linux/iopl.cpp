//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Linux implementation of iopl
///
//===----------------------------------------------------------------------===//

#include "src/sys/io/iopl.h"

#include "src/__support/OSUtil/linux/syscall_wrappers/iopl.h"
#include "src/__support/common.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {

// Sets the privilege level for reaching ports. A level of 3 reaches all of
// them, which is more than ioperm can grant.
LLVM_LIBC_FUNCTION(int, iopl, (int level)) {
  ErrorOr<int> ret = linux_syscalls::iopl(level);
  if (!ret) {
    libc_errno = ret.error();
    return -1;
  }
  return 0;
}

} // namespace LIBC_NAMESPACE_DECL
