//===-- Linux implementation of posix_fadvise -----------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/fcntl/posix_fadvise.h"

#include "hdr/types/off_t.h"
#include "src/__support/OSUtil/syscall.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"

#include <sys/syscall.h>

namespace LIBC_NAMESPACE_DECL {

// Tells the kernel how the file is about to be read, which it is free to
// ignore. The error is in the return rather than in errno, which is what
// POSIX asks of the calls named this way.
LLVM_LIBC_FUNCTION(int, posix_fadvise,
                   (int fd, off_t offset, off_t len, int advice)) {
#if __SIZEOF_LONG__ == 8
  int ret =
      LIBC_NAMESPACE::syscall_impl<int>(SYS_fadvise64, fd, offset, len, advice);
#else
  int ret = LIBC_NAMESPACE::syscall_impl<int>(
      SYS_fadvise64_64, fd, static_cast<long>(offset),
      static_cast<long>(offset >> 32), static_cast<long>(len),
      static_cast<long>(len >> 32), advice);
#endif
  return ret < 0 ? -ret : 0;
}

} // namespace LIBC_NAMESPACE_DECL
