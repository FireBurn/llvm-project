//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Linux implementation of ioperm
///
//===----------------------------------------------------------------------===//

#include "src/sys/io/ioperm.h"

#include "src/__support/OSUtil/linux/syscall_wrappers/ioperm.h"
#include "src/__support/common.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {

// Grants or withdraws the right to reach |num| ports starting at |from|.
// Only the first 65536 ports can be given out this way; iopl is what covers
// the rest.
LLVM_LIBC_FUNCTION(int, ioperm,
                   (unsigned long from, unsigned long num, int turn_on)) {
  ErrorOr<int> ret = linux_syscalls::ioperm(from, num, turn_on);
  if (!ret) {
    libc_errno = ret.error();
    return -1;
  }
  return 0;
}

} // namespace LIBC_NAMESPACE_DECL
