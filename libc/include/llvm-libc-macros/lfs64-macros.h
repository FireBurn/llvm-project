//===-- The names large file support gave the ordinary interfaces ---------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_MACROS_LFS64_MACROS_H
#define LLVM_LIBC_MACROS_LFS64_MACROS_H

// LLVM-libc has one set of interfaces and they are already the wide ones, so
// the names a program uses to ask for large file support name the same
// things. musl does the same.
#if defined(_LARGEFILE64_SOURCE) || defined(_GNU_SOURCE)

#include "../__llvm-libc-common.h"
#include "../llvm-libc-types/DIR.h"
#include "../llvm-libc-types/FILE.h"
#include "../llvm-libc-types/mode_t.h"
#include "../llvm-libc-types/off_t.h"
#include "../llvm-libc-types/size_t.h"
#include "../llvm-libc-types/ssize_t.h"

// The names of records. Each stands for a type and, where there is a call of
// the same name, for that as well: the library still defines those three
// symbols, but the name resolving to the ordinary one covers both uses.
#define stat64 stat
#define statfs64 statfs
#define statvfs64 statvfs
#define rlimit64 rlimit
#define dirent64 dirent

struct dirent;
struct rlimit;
struct stat;
struct statfs;
struct statvfs;

__BEGIN_C_DECLS

// The library defines these symbols, for anything already built against a
// library which had them. They are declared rather than written as macros so
// that a program may take the address of one, which sqlite does to build its
// table of the calls it makes.

// <fcntl.h>
int open64(const char *__path, int __flags, ...);
int openat64(int __dir_fd, const char *__path, int __flags, ...);
int creat64(const char *__path, mode_t __mode);
int posix_fallocate64(int __fd, off_t __offset, off_t __length);
int fallocate64(int __fd, int __mode, off_t __offset, off_t __length);

// <unistd.h>
off_t lseek64(int __fd, off_t __offset, int __whence);
ssize_t pread64(int __fd, void *__buf, size_t __count, off_t __offset);
ssize_t pwrite64(int __fd, const void *__buf, size_t __count, off_t __offset);
int truncate64(const char *__path, off_t __length);
int ftruncate64(int __fd, off_t __length);

// <sys/stat.h>
int fstat64(int __fd, struct stat *__statbuf);
int lstat64(const char *__restrict __path, struct stat *__restrict __statbuf);
int fstatat64(int __dir_fd, const char *__restrict __path,
              struct stat *__restrict __statbuf, int __flags);

// <sys/statfs.h> and <sys/statvfs.h>
int fstatfs64(int __fd, struct statfs *__buf);
int fstatvfs64(int __fd, struct statvfs *__buf);

// <sys/resource.h>
int getrlimit64(int __resource, struct rlimit *__limits);
int setrlimit64(int __resource, const struct rlimit *__limits);

// <sys/mman.h> and <sys/sendfile.h>
void *mmap64(void *__addr, size_t __size, int __prot, int __flags, int __fd,
             off_t __offset);
ssize_t sendfile64(int __out_fd, int __in_fd, off_t *__offset, size_t __count);

// <stdio.h>
FILE *fopen64(const char *__restrict __path, const char *__restrict __mode);
FILE *freopen64(const char *__restrict __path, const char *__restrict __mode,
                FILE *__restrict __stream);
FILE *tmpfile64(void);
int fseeko64(FILE *__stream, off_t __offset, int __whence);
off_t ftello64(FILE *__stream);

// <dirent.h>
struct dirent *readdir64(DIR *__dir);
int scandir64(const char *__path, struct dirent ***__namelist,
              int (*__filter)(const struct dirent *),
              int (*__compare)(const struct dirent **, const struct dirent **));
int alphasort64(const struct dirent **__left, const struct dirent **__right);
int versionsort64(const struct dirent **__left, const struct dirent **__right);

__END_C_DECLS

// The rest have no symbol of their own behind them, so the name stands for
// the ordinary one.

// <fcntl.h> and <unistd.h>
#define posix_fadvise64 posix_fadvise
#define lockf64 lockf

// <sys/resource.h>
#define prlimit64 prlimit

// <stdio.h>
#define fgetpos64 fgetpos
#define fsetpos64 fsetpos

// <stdlib.h>
#define mkstemp64 mkstemp
#define mkostemp64 mkostemp
#define mkstemps64 mkstemps
#define mkostemps64 mkostemps

// <dirent.h>
#define readdir64_r readdir_r

// <ftw.h> and <glob.h>
#define ftw64 ftw
#define nftw64 nftw
#define glob64 glob
#define globfree64 globfree

#endif // defined(_LARGEFILE64_SOURCE) || defined(_GNU_SOURCE)

#endif // LLVM_LIBC_MACROS_LFS64_MACROS_H
