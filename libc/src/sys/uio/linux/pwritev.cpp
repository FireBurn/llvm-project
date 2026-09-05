//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Linux implementation of pwritev
///
//===----------------------------------------------------------------------===//

#include "src/sys/uio/pwritev.h"

#include "hdr/types/off_t.h"
#include "hdr/types/ssize_t.h"
#include "hdr/types/struct_iovec.h"
#include "src/__support/OSUtil/linux/syscall_wrappers/pwritev.h"
#include "src/__support/common.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"
namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(ssize_t, pwritev,
                   (int fd, const struct iovec *iov, int iovcnt,
                    off_t offset)) {
  ErrorOr<ssize_t> ret = linux_syscalls::pwritev(fd, iov, iovcnt, offset);
  if (!ret) {
    libc_errno = ret.error();
    return -1;
  }
  return *ret;
}

} // namespace LIBC_NAMESPACE_DECL
