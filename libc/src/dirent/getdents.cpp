//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation of getdents
///
//===----------------------------------------------------------------------===//

#include "src/dirent/getdents.h"

#include "hdr/types/size_t.h"
#include "hdr/types/ssize_t.h"
#include "src/__support/OSUtil/linux/syscall_wrappers/getdents64.h"
#include "src/__support/common.h"
#include "src/__support/error_or.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {

// The name musl gives this. Entries are written in the one layout this
// library has, the same as getdents64, since there is no other on a
// sixty four bit target.
LLVM_LIBC_FUNCTION(ssize_t, getdents, (int fd, void *buf, size_t count)) {
  ErrorOr<ssize_t> ret = linux_syscalls::getdents64(fd, buf, count);
  if (!ret) {
    libc_errno = ret.error();
    return -1;
  }
  return *ret;
}

} // namespace LIBC_NAMESPACE_DECL
