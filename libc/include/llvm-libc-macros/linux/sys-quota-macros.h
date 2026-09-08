//===-- Macros defined in sys/quota.h header file -------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_MACROS_LINUX_SYS_QUOTA_MACROS_H
#define LLVM_LIBC_MACROS_LINUX_SYS_QUOTA_MACROS_H

// Whose quota is being asked about.
#define USRQUOTA 0
#define GRPQUOTA 1
#define PRJQUOTA 2
#define MAXQUOTAS 3

// A command is the operation and the kind of quota packed together.
#define SUBCMDMASK 0x00ff
#define SUBCMDSHIFT 8
#define QCMD(cmd, type) (((cmd) << SUBCMDSHIFT) | ((type) & SUBCMDMASK))

// The operations.
#define Q_SYNC 0x800001
#define Q_QUOTAON 0x800002
#define Q_QUOTAOFF 0x800003
#define Q_GETFMT 0x800004
#define Q_GETINFO 0x800005
#define Q_SETINFO 0x800006
#define Q_GETQUOTA 0x800007
#define Q_SETQUOTA 0x800008
#define Q_GETNEXTQUOTA 0x800009

// Which fields of a struct dqblk are being set or were filled in.
#define QIF_DQBLKSIZE_BITS 10
#define QIF_DQBLKSIZE (1 << QIF_DQBLKSIZE_BITS)

// The bit numbers these are built from are enumerators in the kernel's own
// <linux/quota.h>, so they are not spelled out here: a macro of the same
// name would break that header for anything which includes both. glibc
// leaves them out for the same reason.
#define QIF_BLIMITS 1
#define QIF_SPACE 2
#define QIF_ILIMITS 4
#define QIF_INODES 8
#define QIF_BTIME 16
#define QIF_ITIME 32
#define QIF_LIMITS (QIF_BLIMITS | QIF_ILIMITS)
#define QIF_USAGE (QIF_SPACE | QIF_INODES)
#define QIF_TIMES (QIF_BTIME | QIF_ITIME)
#define QIF_ALL (QIF_LIMITS | QIF_USAGE | QIF_TIMES)

// The same for a struct dqinfo.
#define IIF_BGRACE 1
#define IIF_IGRACE 2
#define IIF_FLAGS 4
#define IIF_ALL (IIF_BGRACE | IIF_IGRACE | IIF_FLAGS)

// The formats the kernel knows.
#define QFMT_VFS_OLD 1
#define QFMT_VFS_V0 2
#define QFMT_OCFS2 3
#define QFMT_VFS_V1 4

#endif // LLVM_LIBC_MACROS_LINUX_SYS_QUOTA_MACROS_H
