//===-- The utmpx database ------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/utmpx/utmpx_db.h"

#include "hdr/fcntl_macros.h"
#include "hdr/stdio_macros.h"
#include "hdr/types/struct_utmpx.h"
#include "src/__support/OSUtil/syscall.h"
#include "src/__support/macros/config.h"
#include <sys/syscall.h>

#ifndef LIBC_COPT_UTMPX_FILE_PATH
#define LIBC_COPT_UTMPX_FILE_PATH "/var/run/utmp"
#endif

namespace LIBC_NAMESPACE_DECL {
namespace utmpx_db {

const char *path = LIBC_COPT_UTMPX_FILE_PATH;
int fd = -1;
struct utmpx entry;

bool rewind() {
  if (fd < 0) {
    // Read and write, so that pututxline can use the same descriptor, but
    // fall back to reading alone since a program which only looks does not
    // need to be able to write.
    fd = LIBC_NAMESPACE::syscall_impl<int>(SYS_openat, AT_FDCWD, path,
                                           O_RDWR | O_CLOEXEC, 0);
    if (fd < 0)
      fd = LIBC_NAMESPACE::syscall_impl<int>(SYS_openat, AT_FDCWD, path,
                                             O_RDONLY | O_CLOEXEC, 0);
    if (fd < 0)
      return false;
  }
  return LIBC_NAMESPACE::syscall_impl<long>(SYS_lseek, fd, 0, SEEK_SET) >= 0;
}

void close() {
  if (fd >= 0)
    LIBC_NAMESPACE::syscall_impl<int>(SYS_close, fd);
  fd = -1;
}

bool read_next() {
  if (fd < 0 && !rewind())
    return false;

  // A record is fixed width, and a short read means the file ends part way
  // through one, which is not a record to hand back.
  char *out = reinterpret_cast<char *>(&entry);
  size_t got = 0;
  while (got < sizeof(entry)) {
    long n = LIBC_NAMESPACE::syscall_impl<long>(SYS_read, fd, out + got,
                                                sizeof(entry) - got);
    if (n <= 0)
      return false;
    got += static_cast<size_t>(n);
  }
  return true;
}

} // namespace utmpx_db
} // namespace LIBC_NAMESPACE_DECL
