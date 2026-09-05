//===-- Macros defined in sys/statvfs.h header file -----------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_MACROS_LINUX_SYS_STATVFS_MACROS_H
#define LLVM_LIBC_MACROS_LINUX_SYS_STATVFS_MACROS_H

// What f_flag says about how the filesystem is mounted.
#define ST_RDONLY 1
#define ST_NOSUID 2
#define ST_NODEV 4
#define ST_NOEXEC 8
#define ST_SYNCHRONOUS 16
#define ST_MANDLOCK 64
#define ST_WRITE 128
#define ST_APPEND 256
#define ST_IMMUTABLE 512
#define ST_NOATIME 1024
#define ST_NODIRATIME 2048
#define ST_RELATIME 4096
#define ST_NOSYMFOLLOW 8192

#endif // LLVM_LIBC_MACROS_LINUX_SYS_STATVFS_MACROS_H
