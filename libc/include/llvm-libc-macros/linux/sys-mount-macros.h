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

#endif // LLVM_LIBC_MACROS_LINUX_SYS_MOUNT_MACROS_H
