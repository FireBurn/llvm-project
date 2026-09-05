//===-- The names large file support gave the ordinary calls ----*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC_COMPAT_LFS64_H
#define LLVM_LIBC_SRC_COMPAT_LFS64_H

#include "hdr/types/mode_t.h"
#include "hdr/types/off_t.h"
#include "hdr/types/size_t.h"
#include "hdr/types/struct_stat.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {

// Where a file offset is sixty four bits wide already, which it is
// everywhere this builds, these are the calls they name.
int open64(const char *path, int flags, ...);
off_t lseek64(int fd, off_t offset, int whence);
void *mmap64(void *addr, size_t size, int prot, int flags, int fd,
             off_t offset);
int stat64(const char *__restrict path, struct stat *__restrict statbuf);
int fstat64(int fd, struct stat *statbuf);
int lstat64(const char *__restrict path, struct stat *__restrict statbuf);

} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC_COMPAT_LFS64_H
