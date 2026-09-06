//===-- Setting aside the large file support names --------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// The public headers state these as macros onto the calls they name, which
// serves anything compiled against them. The symbols themselves still have to
// exist for anything already built against a libc which had them, and this is
// where they are defined, so here the macros are set aside.
//
// Deliberately without an include guard: any header included after it brings
// the macros back, so it is included again after the last of them.

#undef open64
#undef openat64
#undef creat64
#undef posix_fadvise64
#undef posix_fallocate64
#undef fallocate64
#undef lseek64
#undef pread64
#undef pwrite64
#undef truncate64
#undef ftruncate64
#undef lockf64
#undef stat64
#undef fstat64
#undef lstat64
#undef fstatat64
#undef statfs64
#undef fstatfs64
#undef statvfs64
#undef fstatvfs64
#undef rlimit64
#undef getrlimit64
#undef setrlimit64
#undef prlimit64
#undef mmap64
#undef sendfile64
#undef fopen64
#undef freopen64
#undef tmpfile64
#undef fseeko64
#undef ftello64
#undef fgetpos64
#undef fsetpos64
#undef mkstemp64
#undef mkostemp64
#undef mkstemps64
#undef mkostemps64
#undef dirent64
#undef readdir64
#undef readdir64_r
#undef scandir64
#undef alphasort64
#undef versionsort64
#undef ftw64
#undef nftw64
#undef glob64
#undef globfree64
