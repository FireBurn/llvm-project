//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Syscall wrapper for prlimit64.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC___SUPPORT_OSUTIL_LINUX_SYSCALL_WRAPPERS_PRLIMIT_H
#define LLVM_LIBC_SRC___SUPPORT_OSUTIL_LINUX_SYSCALL_WRAPPERS_PRLIMIT_H

#include "hdr/types/pid_t.h"
#include "hdr/types/struct_rlimit.h"
#include "src/__support/OSUtil/linux/syscall.h" // For syscall_checked
#include "src/__support/common.h"
#include "src/__support/error_or.h"
#include "src/__support/macros/config.h"
#include <sys/syscall.h> // For syscall numbers

namespace LIBC_NAMESPACE_DECL {
namespace linux_syscalls {

LIBC_INLINE ErrorOr<int> prlimit(pid_t pid, int resource,
                                 const struct rlimit *new_limit,
                                 struct rlimit *old_limit) {
  return syscall_checked<int>(SYS_prlimit64, pid, resource, new_limit,
                              old_limit);
}

// The calls that read and change the caller's own limits are the ones a
// sandbox lets through; prlimit64 reaches other processes and is refused.
LIBC_INLINE ErrorOr<int> getrlimit(int resource, struct rlimit *limit) {
#ifdef SYS_getrlimit
  return syscall_checked<int>(SYS_getrlimit, resource, limit);
#else
  return prlimit(0, resource, nullptr, limit);
#endif
}

LIBC_INLINE ErrorOr<int> setrlimit(int resource, const struct rlimit *limit) {
#ifdef SYS_setrlimit
  return syscall_checked<int>(SYS_setrlimit, resource, limit);
#else
  return prlimit(0, resource, limit, nullptr);
#endif
}

} // namespace linux_syscalls
} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC___SUPPORT_OSUTIL_LINUX_SYSCALL_WRAPPERS_PRLIMIT_H
