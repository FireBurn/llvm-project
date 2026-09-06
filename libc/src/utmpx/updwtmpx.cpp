//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation of updwtmpx
///
//===----------------------------------------------------------------------===//

#include "src/utmpx/updwtmpx.h"

#include "hdr/fcntl_macros.h"
#include "hdr/types/struct_utmpx.h"
#include "src/__support/OSUtil/linux/syscall_wrappers/close.h"
#include "src/__support/OSUtil/linux/syscall_wrappers/openat.h"
#include "src/__support/OSUtil/linux/syscall_wrappers/write.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"
namespace LIBC_NAMESPACE_DECL {

// This appends to a log of records rather than to the table of who is
// logged in, so it opens the file for itself and leaves any iteration
// alone. There is no way to report a failure.
namespace internal {

void updwtmpx_impl(const char *wtmpx_file, const struct utmpx *utmpx) {
  if (wtmpx_file == nullptr || utmpx == nullptr)
    return;

  ErrorOr<int> opened = linux_syscalls::openat(
      AT_FDCWD, wtmpx_file, O_WRONLY | O_APPEND | O_CLOEXEC, 0);
  if (!opened)
    return;
  int fd = *opened;

  const char *p = reinterpret_cast<const char *>(utmpx);
  size_t done = 0;
  while (done < sizeof(*utmpx)) {
    ErrorOr<ssize_t> n =
        linux_syscalls::write(fd, p + done, sizeof(*utmpx) - done);
    if (!n || *n <= 0)
      break;
    done += static_cast<size_t>(*n);
  }
  linux_syscalls::close(fd);
}

} // namespace internal

LLVM_LIBC_FUNCTION(void, updwtmpx,
                   (const char *wtmpx_file, const struct utmpx *utmpx)) {
  internal::updwtmpx_impl(wtmpx_file, utmpx);
}

} // namespace LIBC_NAMESPACE_DECL
