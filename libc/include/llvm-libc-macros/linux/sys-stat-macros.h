//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Linux specific declarations of macros from sys/stat.h.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_MACROS_LINUX_SYS_STAT_MACROS_H
#define LLVM_LIBC_MACROS_LINUX_SYS_STAT_MACROS_H

// Definitions from linux/stat.h
#define S_IFMT 0170000
#define S_IFSOCK 0140000
#define S_IFLNK  0120000
#define S_IFREG  0100000
#define S_IFBLK  0060000
#define S_IFDIR  0040000
#define S_IFCHR  0020000
#define S_IFIFO  0010000
#define S_ISUID  0004000
#define S_ISGID  0002000
#define S_ISVTX  0001000

#define S_ISLNK(m)      (((m) & S_IFMT) == S_IFLNK)
#define S_ISREG(m)      (((m) & S_IFMT) == S_IFREG)
#define S_ISDIR(m)      (((m) & S_IFMT) == S_IFDIR)
#define S_ISCHR(m)      (((m) & S_IFMT) == S_IFCHR)
#define S_ISBLK(m)      (((m) & S_IFMT) == S_IFBLK)
#define S_ISFIFO(m)     (((m) & S_IFMT) == S_IFIFO)
#define S_ISSOCK(m)     (((m) & S_IFMT) == S_IFSOCK)

#define S_IRWXU 00700
#define S_IRUSR 00400
#define S_IWUSR 00200
#define S_IXUSR 00100

#define S_IRWXG 00070
#define S_IRGRP 00040
#define S_IWGRP 00020
#define S_IXGRP 00010

#define S_IRWXO 00007
#define S_IROTH 00004
#define S_IWOTH 00002
#define S_IXOTH 00001

#define S_IREAD S_IRUSR
#define S_IWRITE S_IWUSR
#define S_IEXEC S_IXUSR

#define ACCESSPERMS (S_IRWXU | S_IRWXG | S_IRWXO)

#define UTIME_NOW ((1L << 30) - 1L)
#define UTIME_OMIT ((1L << 30) - 2L)

// What a statx caller asks for, and what it is told it got back.
#define STATX_TYPE 0x00000001U
#define STATX_MODE 0x00000002U
#define STATX_NLINK 0x00000004U
#define STATX_UID 0x00000008U
#define STATX_GID 0x00000010U
#define STATX_ATIME 0x00000020U
#define STATX_MTIME 0x00000040U
#define STATX_CTIME 0x00000080U
#define STATX_INO 0x00000100U
#define STATX_SIZE 0x00000200U
#define STATX_BLOCKS 0x00000400U
#define STATX_BASIC_STATS 0x000007ffU
#define STATX_BTIME 0x00000800U
#define STATX_MNT_ID 0x00001000U
#define STATX_DIOALIGN 0x00002000U
#define STATX_MNT_ID_UNIQUE 0x00004000U
#define STATX_SUBVOL 0x00008000U
#define STATX_WRITE_ATOMIC 0x00010000U
#define STATX_DIO_READ_ALIGN 0x00020000U
#define STATX_ALL 0x00000fffU

// What stx_attributes says about the file. These are a separate set from
// the mask above.
#define STATX_ATTR_COMPRESSED 0x00000004
#define STATX_ATTR_IMMUTABLE 0x00000010
#define STATX_ATTR_APPEND 0x00000020
#define STATX_ATTR_NODUMP 0x00000040
#define STATX_ATTR_ENCRYPTED 0x00000800
#define STATX_ATTR_AUTOMOUNT 0x00001000
#define STATX_ATTR_MOUNT_ROOT 0x00002000
#define STATX_ATTR_VERITY 0x00100000
#define STATX_ATTR_DAX 0x00200000
#define STATX_ATTR_WRITE_ATOMIC 0x00400000

#endif // LLVM_LIBC_MACROS_LINUX_SYS_STAT_MACROS_H
