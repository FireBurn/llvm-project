//===-- The names large file support gave the ordinary calls --------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/compat/lfs64.h"

#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/fcntl/open.h"
#include "src/sys/mman/mmap.h"
#include "src/sys/stat/fstat.h"
#include "src/sys/stat/lstat.h"
#include "src/sys/stat/stat.h"
#include "src/unistd/lseek.h"

#include <stdarg.h>

namespace LIBC_NAMESPACE_DECL {

// A file offset has always been sixty four bits wide on the targets this
// builds for, so each of these is the call it is named after. They are here
// because a great deal of code was built against headers that renamed the
// ordinary calls to these, and asks for them by name.

LLVM_LIBC_FUNCTION(int, open64, (const char *path, int flags, ...)) {
  mode_t mode = 0;
  va_list args;
  va_start(args, flags);
  mode = static_cast<mode_t>(va_arg(args, int));
  va_end(args);
  return LIBC_NAMESPACE::open(path, flags, mode);
}

LLVM_LIBC_FUNCTION(off_t, lseek64, (int fd, off_t offset, int whence)) {
  return LIBC_NAMESPACE::lseek(fd, offset, whence);
}

LLVM_LIBC_FUNCTION(void *, mmap64,
                   (void *addr, size_t size, int prot, int flags, int fd,
                    off_t offset)) {
  return LIBC_NAMESPACE::mmap(addr, size, prot, flags, fd, offset);
}

LLVM_LIBC_FUNCTION(int, stat64,
                   (const char *__restrict path, struct stat *__restrict statbuf)) {
  return LIBC_NAMESPACE::stat(path, statbuf);
}

LLVM_LIBC_FUNCTION(int, fstat64, (int fd, struct stat *statbuf)) {
  return LIBC_NAMESPACE::fstat(fd, statbuf);
}

LLVM_LIBC_FUNCTION(int, lstat64,
                   (const char *__restrict path, struct stat *__restrict statbuf)) {
  return LIBC_NAMESPACE::lstat(path, statbuf);
}

} // namespace LIBC_NAMESPACE_DECL
