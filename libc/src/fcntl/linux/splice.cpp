//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Linux implementation of splice
///
//===----------------------------------------------------------------------===//

#include "src/fcntl/splice.h"

#include "src/__support/OSUtil/linux/syscall_wrappers/splice.h"
#include "src/__support/common.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"
namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(ssize_t, splice,
                   (int fd_in, off_t *off_in, int fd_out, off_t *off_out,
                    size_t len, unsigned int flags)) {
  ErrorOr<ssize_t> ret =
      linux_syscalls::splice(fd_in, off_in, fd_out, off_out, len, flags);
  if (!ret) {
    libc_errno = ret.error();
    return -1;
  }
  return *ret;
}

} // namespace LIBC_NAMESPACE_DECL
