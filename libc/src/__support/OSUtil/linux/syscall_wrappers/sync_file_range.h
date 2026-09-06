//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Syscall wrapper for sync_file_range.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC___SUPPORT_OSUTIL_SYSCALL_WRAPPERS_SYNC_FILE_RANGE_H
#define LLVM_LIBC_SRC___SUPPORT_OSUTIL_SYSCALL_WRAPPERS_SYNC_FILE_RANGE_H

#include "hdr/types/off_t.h"
#include "src/__support/OSUtil/linux/syscall.h" // syscall_checked
#include "src/__support/common.h"
#include "src/__support/error_or.h"
#include "src/__support/macros/config.h"
#include <sys/syscall.h> // For syscall numbers

namespace LIBC_NAMESPACE_DECL {
namespace linux_syscalls {

LIBC_INLINE ErrorOr<int> sync_file_range(int fd, off_t offset, off_t nbytes,
                                         unsigned int flags) {
#if __SIZEOF_LONG__ == 8
  return syscall_checked<int>(SYS_sync_file_range, fd, offset, nbytes, flags);
#else
  return syscall_checked<int>(
      SYS_sync_file_range2, fd, flags, static_cast<long>(offset),
      static_cast<long>(offset >> 32), static_cast<long>(nbytes),
      static_cast<long>(nbytes >> 32));
#endif
}

} // namespace linux_syscalls
} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC___SUPPORT_OSUTIL_SYSCALL_WRAPPERS_SYNC_FILE_RANGE_H
