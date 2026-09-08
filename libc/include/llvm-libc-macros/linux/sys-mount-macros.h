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

// The MS_* mount flags are defined by the kernel UAPI header.
#include <linux/mount.h>

// Flags for umount2. The kernel does not export these in a UAPI header.
#define MNT_FORCE 0x1
#define MNT_DETACH 0x2
#define MNT_EXPIRE 0x4
#define UMOUNT_NOFOLLOW 0x8

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
