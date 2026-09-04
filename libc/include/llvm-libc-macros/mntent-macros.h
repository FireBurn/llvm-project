//===-- Macros defined in mntent.h -----------------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_MACROS_MNTENT_MACROS_H
#define LLVM_LIBC_MACROS_MNTENT_MACROS_H

// The file listing the filesystems which can be mounted.
#define MNTTAB "/etc/fstab"

// The file listing the filesystems which are mounted.
#define MOUNTED "/etc/mtab"

// Values for mnt_type.
#define MNTTYPE_IGNORE "ignore"
#define MNTTYPE_NFS "nfs"
#define MNTTYPE_SWAP "swap"

// Values for mnt_opts.
#define MNTOPT_DEFAULTS "defaults"
#define MNTOPT_RO "ro"
#define MNTOPT_RW "rw"
#define MNTOPT_SUID "suid"
#define MNTOPT_NOSUID "nosuid"
#define MNTOPT_NOAUTO "noauto"

#endif // LLVM_LIBC_MACROS_MNTENT_MACROS_H
