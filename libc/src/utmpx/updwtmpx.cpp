//===-- Implementation of updwtmpx ----------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/utmpx/updwtmpx.h"

#include "hdr/fcntl_macros.h"
#include "hdr/types/struct_utmpx.h"
#include "src/__support/OSUtil/syscall.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include <sys/syscall.h>

namespace LIBC_NAMESPACE_DECL {

// This appends to a log of records rather than to the table of who is
// logged in, so it opens the file for itself and leaves any iteration
// alone. There is no way to report a failure.
LLVM_LIBC_FUNCTION(void, updwtmpx,
                   (const char *wtmpx_file, const struct utmpx *utmpx)) {
  if (wtmpx_file == nullptr || utmpx == nullptr)
    return;

  int fd = LIBC_NAMESPACE::syscall_impl<int>(
      SYS_openat, AT_FDCWD, wtmpx_file, O_WRONLY | O_APPEND | O_CLOEXEC, 0);
  if (fd < 0)
    return;

  const char *p = reinterpret_cast<const char *>(utmpx);
  size_t done = 0;
  while (done < sizeof(*utmpx)) {
    long n = LIBC_NAMESPACE::syscall_impl<long>(SYS_write, fd, p + done,
                                                sizeof(*utmpx) - done);
    if (n <= 0)
      break;
    done += static_cast<size_t>(n);
  }
  LIBC_NAMESPACE::syscall_impl<int>(SYS_close, fd);
}

} // namespace LIBC_NAMESPACE_DECL
