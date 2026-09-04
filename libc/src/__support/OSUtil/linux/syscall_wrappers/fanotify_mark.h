//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Syscall wrapper for fanotify_mark.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC___SUPPORT_OSUTIL_SYSCALL_WRAPPERS_FANOTIFY_MARK_H
#define LLVM_LIBC_SRC___SUPPORT_OSUTIL_SYSCALL_WRAPPERS_FANOTIFY_MARK_H

#include "hdr/stdint_proxy.h"
#include "src/__support/OSUtil/linux/syscall.h" // syscall_checked
#include "src/__support/common.h"
#include "src/__support/error_or.h"
#include "src/__support/macros/config.h"
#include <sys/syscall.h> // For syscall numbers

namespace LIBC_NAMESPACE_DECL {
namespace linux_syscalls {

LIBC_INLINE ErrorOr<int> fanotify_mark(int fanotify_fd, unsigned int flags,
                                       uint64_t mask, int dirfd,
                                       const char *pathname) {
#if defined(SYS_fanotify_mark) && __SIZEOF_LONG__ == 8
  return syscall_checked<int>(SYS_fanotify_mark, fanotify_fd, flags, mask,
                              dirfd, pathname);
#else
  // A 32 bit call has to split the mask over two registers, low half first.
  return syscall_checked<int>(
      SYS_fanotify_mark, fanotify_fd, flags, static_cast<uint32_t>(mask),
      static_cast<uint32_t>(mask >> 32), dirfd, pathname);
#endif
}

} // namespace linux_syscalls
} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC___SUPPORT_OSUTIL_SYSCALL_WRAPPERS_FANOTIFY_MARK_H
