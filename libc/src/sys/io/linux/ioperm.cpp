//===-- Linux implementation of ioperm ------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/sys/io/ioperm.h"

#include "src/__support/OSUtil/syscall.h"
#include "src/__support/common.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"

#include <sys/syscall.h>

namespace LIBC_NAMESPACE_DECL {

// Grants or withdraws the right to reach |num| ports starting at |from|.
// Only the first 65536 ports can be given out this way; iopl is what covers
// the rest.
LLVM_LIBC_FUNCTION(int, ioperm,
                   (unsigned long from, unsigned long num, int turn_on)) {
  int ret = LIBC_NAMESPACE::syscall_impl<int>(SYS_ioperm, from, num, turn_on);
  if (ret < 0) {
    libc_errno = -ret;
    return -1;
  }
  return 0;
}

} // namespace LIBC_NAMESPACE_DECL
