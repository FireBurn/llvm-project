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
//
// The calls are function-like so that the name only stands for the call
// where one is being made. A plain macro would also replace the name where
// it is being pasted into another, which is how libseccomp writes a rule:
// SCMP_SYS(pread64) has to reach __SNR_pread64 rather than __SNR_pread. The
// names of records have nothing to pass and stay plain.
#if defined(_LARGEFILE64_SOURCE) || defined(_GNU_SOURCE)

// The records.
#define stat64 stat
#define statfs64 statfs
#define statvfs64 statvfs
#define rlimit64 rlimit
#define dirent64 dirent

// <fcntl.h>
#define open64(...) open(__VA_ARGS__)
#define openat64(...) openat(__VA_ARGS__)
#define creat64(...) creat(__VA_ARGS__)
#define posix_fadvise64(...) posix_fadvise(__VA_ARGS__)
#define posix_fallocate64(...) posix_fallocate(__VA_ARGS__)
#define fallocate64(...) fallocate(__VA_ARGS__)

// <unistd.h>
#define lseek64(...) lseek(__VA_ARGS__)
#define pread64(...) pread(__VA_ARGS__)
#define pwrite64(...) pwrite(__VA_ARGS__)
#define truncate64(...) truncate(__VA_ARGS__)
#define ftruncate64(...) ftruncate(__VA_ARGS__)
#define lockf64(...) lockf(__VA_ARGS__)

// <sys/stat.h>
#define fstat64(...) fstat(__VA_ARGS__)
#define fstatat64(...) fstatat(__VA_ARGS__)
#define lstat64(...) lstat(__VA_ARGS__)

// <sys/statfs.h> and <sys/statvfs.h>
#define fstatfs64(...) fstatfs(__VA_ARGS__)
#define fstatvfs64(...) fstatvfs(__VA_ARGS__)

// <sys/resource.h>
#define getrlimit64(...) getrlimit(__VA_ARGS__)
#define setrlimit64(...) setrlimit(__VA_ARGS__)
#define prlimit64(...) prlimit(__VA_ARGS__)

// <sys/mman.h> and <sys/sendfile.h>
#define mmap64(...) mmap(__VA_ARGS__)
#define sendfile64(...) sendfile(__VA_ARGS__)

// <stdio.h>
#define fopen64(...) fopen(__VA_ARGS__)
#define freopen64(...) freopen(__VA_ARGS__)
#define tmpfile64(...) tmpfile(__VA_ARGS__)
#define fseeko64(...) fseeko(__VA_ARGS__)
#define ftello64(...) ftello(__VA_ARGS__)
#define fgetpos64(...) fgetpos(__VA_ARGS__)
#define fsetpos64(...) fsetpos(__VA_ARGS__)

// <stdlib.h>
#define mkstemp64(...) mkstemp(__VA_ARGS__)
#define mkostemp64(...) mkostemp(__VA_ARGS__)
#define mkstemps64(...) mkstemps(__VA_ARGS__)
#define mkostemps64(...) mkostemps(__VA_ARGS__)

// <dirent.h>
#define readdir64(...) readdir(__VA_ARGS__)
#define readdir64_r(...) readdir_r(__VA_ARGS__)
#define scandir64(...) scandir(__VA_ARGS__)
#define alphasort64(...) alphasort(__VA_ARGS__)
#define versionsort64(...) versionsort(__VA_ARGS__)

// <ftw.h> and <glob.h>
#define ftw64(...) ftw(__VA_ARGS__)
#define nftw64(...) nftw(__VA_ARGS__)
#define glob64(...) glob(__VA_ARGS__)
#define globfree64(...) globfree(__VA_ARGS__)

#endif // defined(_LARGEFILE64_SOURCE) || defined(_GNU_SOURCE)

#endif // LLVM_LIBC_MACROS_LFS64_MACROS_H
