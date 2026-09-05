//===-- Linux implementation of posix_fallocate ---------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/fcntl/posix_fallocate.h"

#include "hdr/errno_macros.h"
#include "hdr/types/off_t.h"
#include "src/__support/OSUtil/syscall.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include <sys/syscall.h>

namespace LIBC_NAMESPACE_DECL {

// The same reservation fallocate makes, but reporting the error in the
// return rather than through errno, which is what POSIX asks of the calls
// named this way.
LLVM_LIBC_FUNCTION(int, posix_fallocate, (int fd, off_t offset, off_t len)) {
  if (len <= 0 || offset < 0)
    return EINVAL;

#if __SIZEOF_LONG__ == 8
  int ret =
      LIBC_NAMESPACE::syscall_impl<int>(SYS_fallocate, fd, 0, offset, len);
#else
  int ret = LIBC_NAMESPACE::syscall_impl<int>(
      SYS_fallocate, fd, 0, static_cast<long>(offset),
      static_cast<long>(offset >> 32), static_cast<long>(len),
      static_cast<long>(len >> 32));
#endif
  return ret < 0 ? -ret : 0;
}

} // namespace LIBC_NAMESPACE_DECL
