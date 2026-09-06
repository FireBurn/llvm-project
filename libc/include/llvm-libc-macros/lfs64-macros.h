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
// things. musl does the same. The library still defines the 64 symbols
// themselves, for anything already built against a libc which had them.
#if defined(_LARGEFILE64_SOURCE) || defined(_GNU_SOURCE)

// The records.
#define stat64 stat
#define statfs64 statfs
#define statvfs64 statvfs
#define rlimit64 rlimit
#define dirent64 dirent

// <fcntl.h>
#define open64 open
#define openat64 openat
#define creat64 creat
#define posix_fadvise64 posix_fadvise
#define posix_fallocate64 posix_fallocate
#define fallocate64 fallocate

// <unistd.h>
#define lseek64 lseek
#define pread64 pread
#define pwrite64 pwrite
#define truncate64 truncate
#define ftruncate64 ftruncate
#define lockf64 lockf

// <sys/stat.h>
#define fstat64 fstat
#define fstatat64 fstatat
#define lstat64 lstat

// <sys/statfs.h> and <sys/statvfs.h>
#define fstatfs64 fstatfs
#define fstatvfs64 fstatvfs

// <sys/resource.h>
#define getrlimit64 getrlimit
#define setrlimit64 setrlimit
#define prlimit64 prlimit

// <sys/mman.h> and <sys/sendfile.h>
#define mmap64 mmap
#define sendfile64 sendfile

// <stdio.h>
#define fopen64 fopen
#define freopen64 freopen
#define tmpfile64 tmpfile
#define fseeko64 fseeko
#define ftello64 ftello
#define fgetpos64 fgetpos
#define fsetpos64 fsetpos

// <stdlib.h>
#define mkstemp64 mkstemp
#define mkostemp64 mkostemp
#define mkstemps64 mkstemps
#define mkostemps64 mkostemps

// <dirent.h>
#define readdir64 readdir
#define readdir64_r readdir_r
#define scandir64 scandir
#define alphasort64 alphasort
#define versionsort64 versionsort

// <ftw.h> and <glob.h>
#define ftw64 ftw
#define nftw64 nftw
#define glob64 glob
#define globfree64 globfree

#endif // defined(_LARGEFILE64_SOURCE) || defined(_GNU_SOURCE)

#endif // LLVM_LIBC_MACROS_LFS64_MACROS_H
