//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Linux implementation of sync_file_range
///
//===----------------------------------------------------------------------===//

#include "src/fcntl/sync_file_range.h"

#include "hdr/types/off_t.h"
#include "src/__support/OSUtil/linux/syscall_wrappers/sync_file_range.h"
#include "src/__support/common.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {

// Writes back part of a file without the whole file sync that fsync does.
// Nothing about it is guaranteed: it is a request that the pages be started
// on their way out, not a promise that they arrived.
LLVM_LIBC_FUNCTION(int, sync_file_range,
                   (int fd, off_t offset, off_t nbytes, unsigned int flags)) {
  ErrorOr<int> ret = linux_syscalls::sync_file_range(fd, offset, nbytes, flags);
  if (!ret) {
    libc_errno = ret.error();
    return -1;
  }
  return 0;
}

} // namespace LIBC_NAMESPACE_DECL
