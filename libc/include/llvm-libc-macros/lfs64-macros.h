//===-- The names large file support gave the ordinary interfaces ---------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// LLVM-libc has one set of interfaces and they are already the wide ones, so
// the names a program uses to ask for large file support name the same
// things. musl does the same.
//
// Each name is declared by the header glibc declares it in, and only there:
// a program which declares one of them itself, as the sanitizers do for the
// calls they intercept, must not meet a second declaration from a header it
// never asked for. So there is no guard over the whole file, which each of
// those headers includes, but one over each header's part of it, taken once
// that header's own guard is defined.

#if defined(_LARGEFILE64_SOURCE) || defined(_GNU_SOURCE)

#ifndef LLVM_LIBC_MACROS_LFS64_MACROS_H
#define LLVM_LIBC_MACROS_LFS64_MACROS_H

#include "../__llvm-libc-common.h"
#include "../llvm-libc-types/DIR.h"
#include "../llvm-libc-types/FILE.h"
#include "../llvm-libc-types/mode_t.h"
#include "../llvm-libc-types/off_t.h"
#include "../llvm-libc-types/size_t.h"
#include "../llvm-libc-types/ssize_t.h"

// The names of records are not here. A macro is in force over a whole
// translation unit, so one standing for a record name would rewrite that name
// wherever it appeared, including in the Linux headers, which define records
// of their own called struct stat64 and struct statfs64. Each of those names
// is defined alongside the record it stands for instead, so that only a
// program which included that record's header is affected.

struct dirent;
struct rlimit;
struct stat;
struct statfs;
struct statvfs;

#endif // LLVM_LIBC_MACROS_LFS64_MACROS_H

// The library defines the symbols declared below, for anything already built
// against a library which had them. They are declared rather than written as
// macros so that a program may take the address of one, which sqlite does to
// build its table of the calls it makes. The rest have no symbol of their
// own behind them, so the name stands for the ordinary one.

#if defined(_LLVM_LIBC_FCNTL_H) && !defined(LLVM_LIBC_LFS64_FCNTL)
#define LLVM_LIBC_LFS64_FCNTL
__BEGIN_C_DECLS
int open64(const char *__path, int __flags, ...);
int openat64(int __dir_fd, const char *__path, int __flags, ...);
int creat64(const char *__path, mode_t __mode);
int posix_fallocate64(int __fd, off_t __offset, off_t __length);
int fallocate64(int __fd, int __mode, off_t __offset, off_t __length);
__END_C_DECLS
#define posix_fadvise64 posix_fadvise
#define lockf64 lockf
#endif

#if defined(_LLVM_LIBC_UNISTD_H) && !defined(LLVM_LIBC_LFS64_UNISTD)
#define LLVM_LIBC_LFS64_UNISTD
__BEGIN_C_DECLS
off_t lseek64(int __fd, off_t __offset, int __whence);
ssize_t pread64(int __fd, void *__buf, size_t __count, off_t __offset);
ssize_t pwrite64(int __fd, const void *__buf, size_t __count, off_t __offset);
int truncate64(const char *__path, off_t __length);
int ftruncate64(int __fd, off_t __length);
__END_C_DECLS
#define lockf64 lockf
#endif

#if defined(_LLVM_LIBC_SYS_STAT_H) && !defined(LLVM_LIBC_LFS64_SYS_STAT)
#define LLVM_LIBC_LFS64_SYS_STAT
__BEGIN_C_DECLS
int fstat64(int __fd, struct stat *__statbuf);
int lstat64(const char *__restrict __path, struct stat *__restrict __statbuf);
int fstatat64(int __dir_fd, const char *__restrict __path,
              struct stat *__restrict __statbuf, int __flags);
__END_C_DECLS
#endif

#if defined(_LLVM_LIBC_SYS_STATFS_H) && !defined(LLVM_LIBC_LFS64_SYS_STATFS)
#define LLVM_LIBC_LFS64_SYS_STATFS
__BEGIN_C_DECLS
int fstatfs64(int __fd, struct statfs *__buf);
__END_C_DECLS
#endif

#if defined(_LLVM_LIBC_SYS_STATVFS_H) && !defined(LLVM_LIBC_LFS64_SYS_STATVFS)
#define LLVM_LIBC_LFS64_SYS_STATVFS
__BEGIN_C_DECLS
int fstatvfs64(int __fd, struct statvfs *__buf);
__END_C_DECLS
#endif

#if defined(_LLVM_LIBC_SYS_RESOURCE_H) &&                                     \
    !defined(LLVM_LIBC_LFS64_SYS_RESOURCE)
#define LLVM_LIBC_LFS64_SYS_RESOURCE
__BEGIN_C_DECLS
int getrlimit64(int __resource, struct rlimit *__limits);
int setrlimit64(int __resource, const struct rlimit *__limits);
__END_C_DECLS
#define prlimit64 prlimit
#endif

#if defined(_LLVM_LIBC_SYS_MMAN_H) && !defined(LLVM_LIBC_LFS64_SYS_MMAN)
#define LLVM_LIBC_LFS64_SYS_MMAN
__BEGIN_C_DECLS
void *mmap64(void *__addr, size_t __size, int __prot, int __flags, int __fd,
             off_t __offset);
__END_C_DECLS
#endif

#if defined(_LLVM_LIBC_SYS_SENDFILE_H) &&                                     \
    !defined(LLVM_LIBC_LFS64_SYS_SENDFILE)
#define LLVM_LIBC_LFS64_SYS_SENDFILE
__BEGIN_C_DECLS
ssize_t sendfile64(int __out_fd, int __in_fd, off_t *__offset, size_t __count);
__END_C_DECLS
#endif

#if defined(_LLVM_LIBC_STDIO_H) && !defined(LLVM_LIBC_LFS64_STDIO)
#define LLVM_LIBC_LFS64_STDIO
__BEGIN_C_DECLS
FILE *fopen64(const char *__restrict __path, const char *__restrict __mode);
FILE *freopen64(const char *__restrict __path, const char *__restrict __mode,
                FILE *__restrict __stream);
FILE *tmpfile64(void);
int fseeko64(FILE *__stream, off_t __offset, int __whence);
off_t ftello64(FILE *__stream);
__END_C_DECLS
#define fgetpos64 fgetpos
#define fsetpos64 fsetpos
#endif

#if defined(_LLVM_LIBC_STDLIB_H) && !defined(LLVM_LIBC_LFS64_STDLIB)
#define LLVM_LIBC_LFS64_STDLIB
#define mkstemp64 mkstemp
#define mkostemp64 mkostemp
#define mkstemps64 mkstemps
#define mkostemps64 mkostemps
#endif

#if defined(_LLVM_LIBC_DIRENT_H) && !defined(LLVM_LIBC_LFS64_DIRENT)
#define LLVM_LIBC_LFS64_DIRENT
__BEGIN_C_DECLS
struct dirent *readdir64(DIR *__dir);
int scandir64(const char *__path, struct dirent ***__namelist,
              int (*__filter)(const struct dirent *),
              int (*__compare)(const struct dirent **, const struct dirent **));
int alphasort64(const struct dirent **__left, const struct dirent **__right);
int versionsort64(const struct dirent **__left, const struct dirent **__right);
__END_C_DECLS
#define readdir64_r readdir_r
#endif

#if defined(_LLVM_LIBC_FTW_H) && !defined(LLVM_LIBC_LFS64_FTW)
#define LLVM_LIBC_LFS64_FTW
#define ftw64 ftw
#define nftw64 nftw
#endif

#if defined(_LLVM_LIBC_GLOB_H) && !defined(LLVM_LIBC_LFS64_GLOB)
#define LLVM_LIBC_LFS64_GLOB
#define glob64 glob
#define globfree64 globfree
#endif

#endif // defined(_LARGEFILE64_SOURCE) || defined(_GNU_SOURCE)
