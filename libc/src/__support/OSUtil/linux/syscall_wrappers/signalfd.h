//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Syscall wrapper for signalfd.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC___SUPPORT_OSUTIL_SYSCALL_WRAPPERS_SIGNALFD_H
#define LLVM_LIBC_SRC___SUPPORT_OSUTIL_SYSCALL_WRAPPERS_SIGNALFD_H

#include "hdr/errno_macros.h"
#include "hdr/types/sigset_t.h"
#include "src/__support/OSUtil/linux/syscall.h" // syscall_checked
#include "src/__support/common.h"
#include "src/__support/error_or.h"
#include "src/__support/macros/config.h"
#include <sys/syscall.h> // For syscall numbers

namespace LIBC_NAMESPACE_DECL {
namespace linux_syscalls {

LIBC_INLINE ErrorOr<int> signalfd(int fd, const sigset_t *mask, int flags) {
#ifdef SYS_signalfd4
  return syscall_checked<int>(SYS_signalfd4, fd, mask, __KERNEL_SIGSET_BYTES,
                              flags);
#elif defined(SYS_signalfd)
  if (flags != 0)
    return Error(EINVAL);
  return syscall_checked<int>(SYS_signalfd, fd, mask, __KERNEL_SIGSET_BYTES);
#else
#error "signalfd and signalfd4 syscalls not available."
#endif
}

} // namespace linux_syscalls
} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC___SUPPORT_OSUTIL_SYSCALL_WRAPPERS_SIGNALFD_H
