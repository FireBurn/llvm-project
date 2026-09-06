//===-- Definition of macros from fcntl.h ---------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_MACROS_LINUX_FCNTL_MACROS_H
#define LLVM_LIBC_MACROS_LINUX_FCNTL_MACROS_H

// POSIX requires the file mode bits to be defined here as well as in
// <sys/stat.h>, since the mode a file is created with is given to open.
#include "../../llvm-libc-macros/linux/sys-stat-macros.h"

// File creation and file status flags.
#define O_APPEND 000002000
#define O_ASYNC 000020000
#define O_CLOEXEC 002000000
#define O_CREAT 000000100
#define O_DSYNC 000010000
#define O_EXCL 000000200
#define O_NOATIME 001000000
#define O_NOCTTY 000000400
#define O_NONBLOCK 000004000
#define O_NDELAY O_NONBLOCK
#define O_PATH 010000000
#define O_SYNC 004010000
#define O_TRUNC 000001000

#ifdef __aarch64__
#define O_DIRECT 000200000
#define O_DIRECTORY 000040000
#define O_NOFOLLOW 000100000
#define O_LARGEFILE 000040000
#define O_TMPFILE 020040000
#else
#define O_DIRECT 000040000
#define O_DIRECTORY 000200000
#define O_NOFOLLOW 000400000
#define O_LARGEFILE 000100000
#define O_TMPFILE 020200000
#endif

// File access mode mask
#define O_ACCMODE 00000003

// File access mode flags
#define O_RDONLY 00000000
#define O_RDWR 00000002
#define O_WRONLY 00000001

// Special directory FD to indicate that the path argument to
// openat is relative to the current directory.
#define AT_FDCWD -100

// Special flag to the function unlinkat to indicate that it
// has to perform the equivalent of "rmdir" on the path argument.
#define AT_REMOVEDIR 0x200

// Special flag for functions like lstat to convey that symlinks
// should not be followed.
#define AT_SYMLINK_NOFOLLOW 0x100

// Allow empty relative pathname.
#define AT_EMPTY_PATH 0x1000

// Perform access checks using the effective user and group IDs.
#define AT_EACCESS 0x200

// Follow the symlink, for the calls whose default is not to.
#define AT_SYMLINK_FOLLOW 0x400

// Do not trigger an automount of the last component.
#define AT_NO_AUTOMOUNT 0x800

// Apply to the whole subtree below the path, not just the path itself.
#define AT_RECURSIVE 0x8000

// How much of a network file's attributes statx has to go and fetch.
#define AT_STATX_SYNC_TYPE 0x6000
#define AT_STATX_SYNC_AS_STAT 0x0000
#define AT_STATX_FORCE_SYNC 0x2000
#define AT_STATX_DONT_SYNC 0x4000

// Ask faccessat2 whether the file could be executed, without running it.
#define AT_EXECVE_CHECK 0x10000

// Values of SYS_fcntl commands.
#define F_DUPFD 0
#define F_GETFD 1
#define F_SETFD 2
#define F_GETFL 3
#define F_SETFL 4
#define F_GETLK 5
#define F_SETLK 6
#define F_SETLKW 7
#define F_SETOWN 8
#define F_GETOWN 9
#define F_SETSIG 10
#define F_GETSIG 11
// The wide forms of the locking commands. Where a file offset is already
// sixty four bits wide, which it is on every target with sixty four bit
// pointers, these are the ordinary commands: the numbered ones are how a
// thirty two bit ABI asks the kernel for the wide struct, and a kernel built
// for the wider one does not answer to them at all.
#ifdef __LP64__
#define F_GETLK64 F_GETLK
#define F_SETLK64 F_SETLK
#define F_SETLKW64 F_SETLKW
#else
#define F_GETLK64 12
#define F_SETLK64 13
#define F_SETLKW64 14
#endif
#define F_SETOWN_EX 15
#define F_GETOWN_EX 16

// Commands past the ones every Unix has. The kernel numbers these from a
// base of its own.
#define F_LINUX_SPECIFIC_BASE 1024
#define F_DUPFD_CLOEXEC (F_LINUX_SPECIFIC_BASE + 6)
#define F_SETPIPE_SZ (F_LINUX_SPECIFIC_BASE + 7)
#define F_GETPIPE_SZ (F_LINUX_SPECIFIC_BASE + 8)
#define F_ADD_SEALS (F_LINUX_SPECIFIC_BASE + 9)
#define F_GET_SEALS (F_LINUX_SPECIFIC_BASE + 10)

// The seals a memfd can be given, which say what may no longer be done to
// it.
#define F_SEAL_SEAL 0x0001
#define F_SEAL_SHRINK 0x0002
#define F_SEAL_GROW 0x0004
#define F_SEAL_WRITE 0x0008
#define F_SEAL_FUTURE_WRITE 0x0010
#define F_SEAL_EXEC 0x0020

// Open File Description Locks.
#define F_OFD_GETLK 36
#define F_OFD_SETLK 37
#define F_OFD_SETLKW 38

// Close on succesful
#define F_CLOEXEC 1

// Close on execute for fcntl.
#define FD_CLOEXEC 1

// What splice and vmsplice may do with the pages they move.
#define SPLICE_F_MOVE 1
#define SPLICE_F_NONBLOCK 2
#define SPLICE_F_MORE 4
#define SPLICE_F_GIFT 8

// What fallocate may do to the range it is given.
#define FALLOC_FL_ALLOCATE_RANGE 0x00
#define FALLOC_FL_KEEP_SIZE 0x01
#define FALLOC_FL_PUNCH_HOLE 0x02
#define FALLOC_FL_NO_HIDE_STALE 0x04
#define FALLOC_FL_COLLAPSE_RANGE 0x08
#define FALLOC_FL_ZERO_RANGE 0x10
#define FALLOC_FL_INSERT_RANGE 0x20
#define FALLOC_FL_UNSHARE_RANGE 0x40
#define FALLOC_FL_WRITE_ZEROES 0x80

#define F_RDLCK 0
#define F_WRLCK 1
#define F_UNLCK 2

// For Large File Support. On a target whose offsets are already wide the
// names above already say the same thing, and saying it again here would
// leave the ordinary names standing for nothing.
#if defined(_LARGEFILE64_SOURCE) && !defined(__LP64__)
#define F_GETLK F_GETLK64
#define F_SETLK F_SETLK64
#define F_SETLKW F_SETLKW64
#endif

// How a file is about to be read, for posix_fadvise. The kernel is free to
// ignore any of them.
#define POSIX_FADV_NORMAL 0
#define POSIX_FADV_RANDOM 1
#define POSIX_FADV_SEQUENTIAL 2
#define POSIX_FADV_WILLNEED 3
#define POSIX_FADV_DONTNEED 4
#define POSIX_FADV_NOREUSE 5

#endif // LLVM_LIBC_MACROS_LINUX_FCNTL_MACROS_H
