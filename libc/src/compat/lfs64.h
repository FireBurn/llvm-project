//===-- The names large file support gave the ordinary calls ----*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC_COMPAT_LFS64_H
#define LLVM_LIBC_SRC_COMPAT_LFS64_H

#include "hdr/types/DIR.h"
#include "hdr/types/FILE.h"
#include "hdr/types/mode_t.h"
#include "hdr/types/off_t.h"
#include "hdr/types/size_t.h"
#include "hdr/types/ssize_t.h"
#include "hdr/types/struct_dirent.h"
#include "hdr/types/struct_rlimit.h"
#include "hdr/types/struct_stat.h"
#include "hdr/types/struct_statfs.h"
#include "include/llvm-libc-types/struct_statvfs.h"
#include "src/__support/macros/config.h"

// dirent.h states these as macros onto the calls they name, which serves
// anything compiled against it. The symbols themselves still have to be here
// for anything already built, so the macros are set aside to define them.
#undef readdir64
#undef scandir64
#undef alphasort64
#undef versionsort64

namespace LIBC_NAMESPACE_DECL {

// Where a file offset is sixty four bits wide already, which it is
// everywhere this builds, these are the calls they name. A great deal of code
// was built against headers that renamed the ordinary calls to these and asks
// for them by name.
int open64(const char *path, int flags, ...);
int openat64(int dir_fd, const char *path, int flags, ...);
int creat64(const char *path, mode_t mode);
off_t lseek64(int fd, off_t offset, int whence);
void *mmap64(void *addr, size_t size, int prot, int flags, int fd,
             off_t offset);
int stat64(const char *__restrict path, struct stat *__restrict statbuf);
int fstat64(int fd, struct stat *statbuf);
int lstat64(const char *__restrict path, struct stat *__restrict statbuf);
int fstatat64(int dir_fd, const char *__restrict path,
              struct stat *__restrict statbuf, int flags);
int ftruncate64(int fd, off_t length);
int truncate64(const char *path, off_t length);
ssize_t pread64(int fd, void *buf, size_t count, off_t offset);
ssize_t pwrite64(int fd, const void *buf, size_t count, off_t offset);
struct dirent *readdir64(DIR *dir);
int scandir64(const char *path, struct dirent ***namelist,
              int (*filter)(const struct dirent *),
              int (*compare)(const struct dirent **, const struct dirent **));
int alphasort64(const struct dirent **left, const struct dirent **right);
int versionsort64(const struct dirent **left, const struct dirent **right);
int statfs64(const char *path, struct statfs *buf);
int fstatfs64(int fd, struct statfs *buf);
int statvfs64(const char *__restrict path, struct statvfs *__restrict buf);
int fstatvfs64(int fd, struct statvfs *buf);
int posix_fallocate64(int fd, off_t offset, off_t length);
int fallocate64(int fd, int mode, off_t offset, off_t length);
int getrlimit64(int resource, struct rlimit *limits);
int setrlimit64(int resource, const struct rlimit *limits);
FILE *tmpfile64(void);
FILE *fopen64(const char *__restrict path, const char *__restrict mode);
FILE *freopen64(const char *__restrict path, const char *__restrict mode,
                FILE *__restrict stream);
int fseeko64(FILE *stream, off_t offset, int whence);
off_t ftello64(FILE *stream);
ssize_t sendfile64(int out_fd, int in_fd, off_t *offset, size_t count);

} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC_COMPAT_LFS64_H
