//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Linux implementation of tee
///
//===----------------------------------------------------------------------===//

#include "src/fcntl/tee.h"

#include "src/__support/OSUtil/linux/syscall_wrappers/tee.h"
#include "src/__support/common.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"
namespace LIBC_NAMESPACE_DECL {

// Copies what one pipe holds into another without taking it out of the first,
// so that the same bytes can be read twice.
LLVM_LIBC_FUNCTION(ssize_t, tee,
                   (int fd_in, int fd_out, size_t len, unsigned int flags)) {
  ErrorOr<ssize_t> ret = linux_syscalls::tee(fd_in, fd_out, len, flags);
  if (!ret) {
    libc_errno = ret.error();
    return -1;
  }
  return *ret;
}

} // namespace LIBC_NAMESPACE_DECL
