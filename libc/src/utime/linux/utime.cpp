//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Linux implementation of utime.
///
//===----------------------------------------------------------------------===//

#include "src/utime/utime.h"

#include "hdr/fcntl_macros.h"
#include "hdr/types/struct_timespec.h"
#include "hdr/types/struct_utimbuf.h"
#include "src/__support/OSUtil/linux/syscall_wrappers/utimensat.h"
#include "src/__support/common.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(int, utime,
                   (const char *path, const struct utimbuf *times)) {
  struct timespec ts[2];
  struct timespec *tsp = nullptr;
  if (times != nullptr) {
    ts[0].tv_sec = times->actime;
    ts[0].tv_nsec = 0;
    ts[1].tv_sec = times->modtime;
    ts[1].tv_nsec = 0;
    tsp = ts;
  }
  // A null |times| means use the current time, which utimensat spells as a
  // null pointer too.
  auto ret = linux_syscalls::utimensat(AT_FDCWD, path, tsp, 0);
  if (!ret) {
    libc_errno = ret.error();
    return -1;
  }
  return 0;
}

} // namespace LIBC_NAMESPACE_DECL
