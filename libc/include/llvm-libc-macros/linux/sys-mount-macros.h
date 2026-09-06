//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Linux specific declarations of macros from sys/mount.h.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_MACROS_LINUX_SYS_MOUNT_MACROS_H
#define LLVM_LIBC_MACROS_LINUX_SYS_MOUNT_MACROS_H

// The newer mount interface is the kernel's own, and a good deal of it is
// spelled as enumerations and structures rather than as macros, so there is
// no way to define it here without breaking <linux/mount.h> for anything
// which includes both. Taking it from the kernel is what glibc does too.
// Everything below is written so that it does not fight what comes out of
// this include.
#if defined(__has_include)
#if __has_include(<linux/mount.h>)
#include <linux/mount.h>
#endif
#endif

#define MS_RDONLY (1 << 0)
#define MS_NOSUID (1 << 1)
#define MS_NODEV (1 << 2)
#define MS_NOEXEC (1 << 3)
#define MS_SYNCHRONOUS (1 << 4)
#define MS_REMOUNT (1 << 5)
#define MS_MANDLOCK (1 << 6)
#define MS_DIRSYNC (1 << 7)
#define MS_NOSYMFOLLOW (1 << 8)
#define MS_NOATIME (1 << 10)
#define MS_NODIRATIME (1 << 11)
#define MS_BIND (1 << 12)
#define MS_MOVE (1 << 13)
#define MS_REC (1 << 14)
#define MS_SILENT (1 << 15)
#define MS_UNBINDABLE (1 << 17)
#define MS_PRIVATE (1 << 18)
#define MS_SLAVE (1 << 19)
#define MS_SHARED (1 << 20)
#define MS_RELATIME (1 << 21)
#define MS_STRICTATIME (1 << 24)
#define MS_LAZYTIME (1 << 25)

// The flags umount2 takes: whether to give up on a filesystem still in use,
// to detach it and clean up later, or to refuse to follow a symbolic link.
#define MNT_FORCE 1
#define MNT_DETACH 2
#define MNT_EXPIRE 4
#define UMOUNT_NOFOLLOW 8

// What mount_setattr may change about a mount, which is a separate set from
// the flags the older mount call takes.
#define MOUNT_ATTR_RDONLY 0x00000001
#define MOUNT_ATTR_NOSUID 0x00000002
#define MOUNT_ATTR_NODEV 0x00000004
#define MOUNT_ATTR_NOEXEC 0x00000008
#define MOUNT_ATTR__ATIME 0x00000070
#define MOUNT_ATTR_RELATIME 0x00000000
#define MOUNT_ATTR_NOATIME 0x00000010
#define MOUNT_ATTR_STRICTATIME 0x00000020
#define MOUNT_ATTR_NODIRATIME 0x00000080
#define MOUNT_ATTR_IDMAP 0x00100000
#define MOUNT_ATTR_NOSYMFOLLOW 0x00200000
#define MOUNT_ATTR_SIZE_VER0 32

// The newer way to mount something, in which a filesystem is set up as a
// context first and only attached to the tree once it is ready.

// Flags to fsopen and fsmount.
#define FSOPEN_CLOEXEC 0x00000001
#define FSMOUNT_CLOEXEC 0x00000001
#define FSMOUNT_NAMESPACE 0x00000002

// What fsconfig is being asked to do is an enumeration in the kernel's
// <linux/mount.h>, and there is no __UAPI_DEF guard for it, so a macro of
// the same name here would break that header for anything which includes
// both. The names are left to the kernel, and so is struct mount_attr,
// which that header declares for the same reason.

// Flags to open_tree.
#define OPEN_TREE_CLONE (1 << 0)
#define OPEN_TREE_NAMESPACE (1 << 1)
#define OPEN_TREE_CLOEXEC O_CLOEXEC

// Flags to move_mount, which has a pair of each for the two paths it takes.
#define MOVE_MOUNT_F_SYMLINKS 0x00000001
#define MOVE_MOUNT_F_AUTOMOUNTS 0x00000002
#define MOVE_MOUNT_F_EMPTY_PATH 0x00000004
#define MOVE_MOUNT_T_SYMLINKS 0x00000010
#define MOVE_MOUNT_T_AUTOMOUNTS 0x00000020
#define MOVE_MOUNT_T_EMPTY_PATH 0x00000040
#define MOVE_MOUNT_SET_GROUP 0x00000100
#define MOVE_MOUNT_BENEATH 0x00000200

// The requests a block device takes. They are the kernel's, given here under
// the names <sys/mount.h> is expected to carry rather than only in
// <linux/fs.h>, and built from the same _IO macros the kernel builds them
// from so that the numbers cannot drift apart.
#include <linux/ioctl.h>

#ifndef BLKROSET
#define BLKROSET _IO(0x12, 93) // Set the device read only.
#endif
#ifndef BLKROGET
#define BLKROGET _IO(0x12, 94) // Whether it is read only.
#endif
#ifndef BLKRRPART
#define BLKRRPART _IO(0x12, 95) // Read the partition table again.
#endif
#ifndef BLKGETSIZE
#define BLKGETSIZE _IO(0x12, 96) // The size in 512 byte sectors.
#endif
#ifndef BLKFLSBUF
#define BLKFLSBUF _IO(0x12, 97) // Write back and drop the buffers.
#endif
#ifndef BLKRASET
#define BLKRASET _IO(0x12, 98) // Set the read ahead.
#endif
#ifndef BLKRAGET
#define BLKRAGET _IO(0x12, 99)
#endif
#ifndef BLKFRASET
#define BLKFRASET _IO(0x12, 100) // Set the file system read ahead.
#endif
#ifndef BLKFRAGET
#define BLKFRAGET _IO(0x12, 101)
#endif
#ifndef BLKSECTSET
#define BLKSECTSET _IO(0x12, 102) // Set the maximum sectors per request.
#endif
#ifndef BLKSECTGET
#define BLKSECTGET _IO(0x12, 103)
#endif
#ifndef BLKSSZGET
#define BLKSSZGET _IO(0x12, 104) // The logical sector size.
#endif
#ifndef BLKBSZGET
#define BLKBSZGET _IOR(0x12, 112, __SIZE_TYPE__)
#endif
#ifndef BLKBSZSET
#define BLKBSZSET _IOW(0x12, 113, __SIZE_TYPE__)
#endif
#ifndef BLKGETSIZE64
#define BLKGETSIZE64 _IOR(0x12, 114, __SIZE_TYPE__) // The size in bytes.
#endif

#endif // LLVM_LIBC_MACROS_LINUX_SYS_MOUNT_MACROS_H
