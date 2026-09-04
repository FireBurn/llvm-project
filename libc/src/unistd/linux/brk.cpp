//===-- Linux implementation of brk ---------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/unistd/brk.h"

#include "hdr/errno_macros.h"
#include "src/__support/OSUtil/linux/syscall_wrappers/brk.h"
#include "src/__support/common.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"
#include "src/unistd/linux/brk_utils.h"

namespace LIBC_NAMESPACE_DECL {

namespace internal {
void *current_break = nullptr;
} // namespace internal

LLVM_LIBC_FUNCTION(int, brk, (void *addr)) {
  void *result = linux_syscalls::brk(addr);
  // The syscall reports what the break is now, so a request it could not
  // meet shows up as a break lower than the one asked for.
  if (result < addr) {
    libc_errno = ENOMEM;
    return -1;
  }
  internal::current_break = result;
  return 0;
}

} // namespace LIBC_NAMESPACE_DECL
