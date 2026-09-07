//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Linux implementation of vmsplice
///
//===----------------------------------------------------------------------===//

#include "src/fcntl/vmsplice.h"

#include "src/__support/OSUtil/linux/syscall_wrappers/vmsplice.h"
#include "src/__support/common.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"
namespace LIBC_NAMESPACE_DECL {

// Moves what the caller's own memory holds into a pipe, or takes what a pipe
// holds into it, without a copy: splice does the same between two
// descriptors, and this is the end of it that is not one.
LLVM_LIBC_FUNCTION(ssize_t, vmsplice,
                   (int fd, const struct iovec *iov, size_t count,
                    unsigned int flags)) {
  ErrorOr<ssize_t> ret = linux_syscalls::vmsplice(fd, iov, count, flags);
  if (!ret) {
    libc_errno = ret.error();
    return -1;
  }
  return *ret;
}

} // namespace LIBC_NAMESPACE_DECL
