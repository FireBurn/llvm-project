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
#include "src/dirent/alphasort.h"
#include "src/dirent/readdir.h"
#include "src/dirent/scandir.h"
#include "src/dirent/versionsort.h"
#include "src/fcntl/creat.h"
#include "src/fcntl/fallocate.h"
#include "src/fcntl/open.h"
#include "src/fcntl/openat.h"
#include "src/fcntl/posix_fallocate.h"
#include "src/stdio/fopen.h"
#include "src/stdio/freopen.h"
#include "src/stdio/fseeko.h"
#include "src/stdio/ftello.h"
#include "src/stdio/tmpfile.h"
#include "src/sys/mman/mmap.h"
#include "src/sys/resource/getrlimit.h"
#include "src/sys/resource/setrlimit.h"
#include "src/sys/sendfile/sendfile.h"
#include "src/sys/stat/fstat.h"
#include "src/sys/stat/fstatat.h"
#include "src/sys/stat/lstat.h"
#include "src/sys/stat/stat.h"
#include "src/sys/statfs/fstatfs.h"
#include "src/sys/statfs/statfs.h"
#include "src/sys/statvfs/fstatvfs.h"
#include "src/sys/statvfs/statvfs.h"
#include "src/unistd/ftruncate.h"
#include "src/unistd/lseek.h"
#include "src/unistd/pread.h"
#include "src/unistd/pwrite.h"
#include "src/unistd/truncate.h"

#include <stdarg.h>

// Brought back by the headers included above.
#include "src/compat/lfs64_undef.h"

// dirent.h states these as macros onto the calls they name, which serves
// anything compiled against it. The symbols themselves still have to be here
// for anything already built, so the macros are set aside to define them.
#undef readdir64
#undef scandir64
#undef alphasort64
#undef versionsort64

namespace LIBC_NAMESPACE_DECL {

// A file offset has always been sixty four bits wide on the targets this
// builds for, so each of these is the call it is named after. They are here
// because a great deal of code was built against headers that renamed the
// ordinary calls to these, and asks for them by name.

LLVM_LIBC_FUNCTION(int, open64, (const char *path, int flags, ...)) {
  va_list args;
  va_start(args, flags);
  mode_t mode = static_cast<mode_t>(va_arg(args, int));
  va_end(args);
  return LIBC_NAMESPACE::open(path, flags, mode);
}

LLVM_LIBC_FUNCTION(int, openat64,
                   (int dir_fd, const char *path, int flags, ...)) {
  va_list args;
  va_start(args, flags);
  mode_t mode = static_cast<mode_t>(va_arg(args, int));
  va_end(args);
  return LIBC_NAMESPACE::openat(dir_fd, path, flags, mode);
}

LLVM_LIBC_FUNCTION(int, creat64, (const char *path, mode_t mode)) {
  return LIBC_NAMESPACE::creat(path, mode);
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
                   (const char *__restrict path,
                    struct stat *__restrict statbuf)) {
  return LIBC_NAMESPACE::stat(path, statbuf);
}

LLVM_LIBC_FUNCTION(int, fstat64, (int fd, struct stat *statbuf)) {
  return LIBC_NAMESPACE::fstat(fd, statbuf);
}

LLVM_LIBC_FUNCTION(int, lstat64,
                   (const char *__restrict path,
                    struct stat *__restrict statbuf)) {
  return LIBC_NAMESPACE::lstat(path, statbuf);
}

LLVM_LIBC_FUNCTION(int, fstatat64,
                   (int dir_fd, const char *__restrict path,
                    struct stat *__restrict statbuf, int flags)) {
  return LIBC_NAMESPACE::fstatat(dir_fd, path, statbuf, flags);
}

LLVM_LIBC_FUNCTION(int, ftruncate64, (int fd, off_t length)) {
  return LIBC_NAMESPACE::ftruncate(fd, length);
}

LLVM_LIBC_FUNCTION(int, truncate64, (const char *path, off_t length)) {
  return LIBC_NAMESPACE::truncate(path, length);
}

LLVM_LIBC_FUNCTION(ssize_t, pread64,
                   (int fd, void *buf, size_t count, off_t offset)) {
  return LIBC_NAMESPACE::pread(fd, buf, count, offset);
}

LLVM_LIBC_FUNCTION(ssize_t, pwrite64,
                   (int fd, const void *buf, size_t count, off_t offset)) {
  return LIBC_NAMESPACE::pwrite(fd, buf, count, offset);
}

LLVM_LIBC_FUNCTION(struct dirent *, readdir64, (DIR * dir)) {
  return LIBC_NAMESPACE::readdir(dir);
}

LLVM_LIBC_FUNCTION(int, scandir64,
                   (const char *path, struct dirent ***namelist,
                    int (*filter)(const struct dirent *),
                    int (*compare)(const struct dirent **,
                                   const struct dirent **))) {
  return LIBC_NAMESPACE::scandir(path, namelist, filter, compare);
}

LLVM_LIBC_FUNCTION(int, alphasort64,
                   (const struct dirent **left, const struct dirent **right)) {
  return LIBC_NAMESPACE::alphasort(left, right);
}

LLVM_LIBC_FUNCTION(int, versionsort64,
                   (const struct dirent **left, const struct dirent **right)) {
  return LIBC_NAMESPACE::versionsort(left, right);
}

LLVM_LIBC_FUNCTION(int, statfs64, (const char *path, struct statfs *buf)) {
  return LIBC_NAMESPACE::statfs(path, buf);
}

LLVM_LIBC_FUNCTION(int, fstatfs64, (int fd, struct statfs *buf)) {
  return LIBC_NAMESPACE::fstatfs(fd, buf);
}

LLVM_LIBC_FUNCTION(int, statvfs64,
                   (const char *__restrict path,
                    struct statvfs *__restrict buf)) {
  return LIBC_NAMESPACE::statvfs(path, buf);
}

LLVM_LIBC_FUNCTION(int, fstatvfs64, (int fd, struct statvfs *buf)) {
  return LIBC_NAMESPACE::fstatvfs(fd, buf);
}

LLVM_LIBC_FUNCTION(int, posix_fallocate64,
                   (int fd, off_t offset, off_t length)) {
  return LIBC_NAMESPACE::posix_fallocate(fd, offset, length);
}

LLVM_LIBC_FUNCTION(int, fallocate64,
                   (int fd, int mode, off_t offset, off_t length)) {
  return LIBC_NAMESPACE::fallocate(fd, mode, offset, length);
}

LLVM_LIBC_FUNCTION(int, getrlimit64, (int resource, struct rlimit *limits)) {
  return LIBC_NAMESPACE::getrlimit(resource, limits);
}

LLVM_LIBC_FUNCTION(int, setrlimit64,
                   (int resource, const struct rlimit *limits)) {
  return LIBC_NAMESPACE::setrlimit(resource, limits);
}

LLVM_LIBC_FUNCTION(FILE *, tmpfile64, (void)) {
  return LIBC_NAMESPACE::tmpfile();
}

LLVM_LIBC_FUNCTION(FILE *, fopen64,
                   (const char *__restrict path, const char *__restrict mode)) {
  return LIBC_NAMESPACE::fopen(path, mode);
}

LLVM_LIBC_FUNCTION(FILE *, freopen64,
                   (const char *__restrict path, const char *__restrict mode,
                    FILE *__restrict stream)) {
  return LIBC_NAMESPACE::freopen(path, mode, stream);
}

LLVM_LIBC_FUNCTION(int, fseeko64, (FILE * stream, off_t offset, int whence)) {
  return LIBC_NAMESPACE::fseeko(stream, offset, whence);
}

LLVM_LIBC_FUNCTION(off_t, ftello64, (FILE * stream)) {
  return LIBC_NAMESPACE::ftello(stream);
}

LLVM_LIBC_FUNCTION(ssize_t, sendfile64,
                   (int out_fd, int in_fd, off_t *offset, size_t count)) {
  return LIBC_NAMESPACE::sendfile(out_fd, in_fd, offset, count);
}

} // namespace LIBC_NAMESPACE_DECL
