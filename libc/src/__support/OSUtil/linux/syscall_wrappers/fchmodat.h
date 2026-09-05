//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// ErrorOr-returning syscall wrapper for fchmodat.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC___SUPPORT_OSUTIL_SYSCALL_WRAPPERS_FCHMODAT_H
#define LLVM_LIBC_SRC___SUPPORT_OSUTIL_SYSCALL_WRAPPERS_FCHMODAT_H

#include "hdr/errno_macros.h"
#include "hdr/types/mode_t.h"
#include "src/__support/OSUtil/linux/syscall.h" // syscall_impl
#include "src/__support/common.h"
#include "src/__support/error_or.h"
#include "src/__support/macros/config.h"
#include <sys/syscall.h> // For syscall numbers

namespace LIBC_NAMESPACE_DECL {
namespace linux_syscalls {

LIBC_INLINE ErrorOr<int> fchmodat(int fd, const char *path, mode_t mode,
                                  int flags) {
  // The fchmodat syscall takes no flags at all, so passing them to it as a
  // fourth argument leaves them ignored: a request not to follow a symbolic
  // link would follow it, and one to act on the descriptor itself would look
  // for a file with an empty name. Anything with flags goes to fchmodat2,
  // which takes them. A kernel without that reports so, rather than doing
  // something other than what was asked.
  int ret;
  if (flags != 0) {
#ifdef SYS_fchmodat2
    ret = syscall_impl<int>(SYS_fchmodat2, fd, path, mode, flags);
#else
    return Error(ENOSYS);
#endif
  } else {
    ret = syscall_impl<int>(SYS_fchmodat, fd, path, mode);
  }
  if (ret < 0)
    return Error(-ret);
  return ret;
}

} // namespace linux_syscalls
} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC___SUPPORT_OSUTIL_SYSCALL_WRAPPERS_FCHMODAT_H
