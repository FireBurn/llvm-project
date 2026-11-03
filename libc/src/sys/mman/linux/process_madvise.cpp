//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation of process_madvise.
///
//===----------------------------------------------------------------------===//

#include "src/__support/OSUtil/linux/syscall_wrappers/process_madvise.h"
#include "hdr/types/size_t.h"
#include "hdr/types/ssize_t.h"
#include "hdr/types/struct_iovec.h"
#include "src/__support/common.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"
#include "src/sys/mman/process_madvise.h"

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(ssize_t, process_madvise,
                   (int pidfd, const struct iovec *iov, size_t vlen, int advice,
                    unsigned int flags)) {
  ErrorOr<ssize_t> ret =
      linux_syscalls::process_madvise(pidfd, iov, vlen, advice, flags);
  if (!ret) {
    libc_errno = ret.error();
    return -1;
  }
  return ret.value();
}

} // namespace LIBC_NAMESPACE_DECL
