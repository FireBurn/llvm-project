//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Definition of struct mntent.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_TYPES_STRUCT_MNTENT_H
#define LLVM_LIBC_TYPES_STRUCT_MNTENT_H

// One entry of a filesystem table such as /etc/fstab or /proc/mounts.
struct mntent {
  char *mnt_fsname; // The device or remote filesystem mounted.
  char *mnt_dir;    // Where it is mounted.
  char *mnt_type;   // The filesystem type.
  char *mnt_opts;   // A comma separated list of mount options.
  int mnt_freq;     // How often to dump, in days.
  int mnt_passno;   // The pass number for a boot time fsck.
};

#endif // LLVM_LIBC_TYPES_STRUCT_MNTENT_H
