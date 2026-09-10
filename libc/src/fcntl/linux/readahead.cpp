//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Linux implementation of readahead
///
//===----------------------------------------------------------------------===//

#include "src/fcntl/readahead.h"

#include "hdr/types/off_t.h"
#include "hdr/types/size_t.h"
#include "hdr/types/ssize_t.h"
#include "src/__support/OSUtil/linux/syscall_wrappers/readahead.h"
#include "src/__support/common.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {

// Asks for a stretch of a file to be brought into the page cache without
// waiting for it. Nothing is read into the caller's memory, so the answer
// says the request was made rather than how much arrived.
LLVM_LIBC_FUNCTION(ssize_t, readahead, (int fd, off_t offset, size_t count)) {
  ErrorOr<ssize_t> ret = linux_syscalls::readahead(fd, offset, count);
  if (!ret) {
    libc_errno = ret.error();
    return -1;
  }
  return *ret;
}

} // namespace LIBC_NAMESPACE_DECL
