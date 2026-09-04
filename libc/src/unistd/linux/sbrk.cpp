//===-- Linux implementation of sbrk --------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/unistd/sbrk.h"

#include "hdr/errno_macros.h"
#include "hdr/stdint_proxy.h"
#include "src/__support/OSUtil/linux/syscall_wrappers/brk.h"
#include "src/__support/common.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"
#include "src/unistd/linux/brk_utils.h"

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(void *, sbrk, (intptr_t increment)) {
  if (internal::current_break == nullptr) {
    // Asking for a break of zero is how the current one is found out.
    internal::current_break = linux_syscalls::brk(nullptr);
    if (internal::current_break == nullptr) {
      libc_errno = ENOMEM;
      return reinterpret_cast<void *>(-1);
    }
  }

  void *old = internal::current_break;
  if (increment == 0)
    return old;

  char *wanted = reinterpret_cast<char *>(old) + increment;
  // Moving the break below where it started, or so far up that the address
  // wraps, is not something the kernel can be asked for.
  if ((increment > 0 && wanted < reinterpret_cast<char *>(old)) ||
      (increment < 0 && wanted > reinterpret_cast<char *>(old))) {
    libc_errno = ENOMEM;
    return reinterpret_cast<void *>(-1);
  }

  void *result = linux_syscalls::brk(wanted);
  if (result != wanted) {
    libc_errno = ENOMEM;
    return reinterpret_cast<void *>(-1);
  }
  internal::current_break = result;
  return old;
}

} // namespace LIBC_NAMESPACE_DECL
