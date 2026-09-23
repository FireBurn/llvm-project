//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Linux implementation for `stat` functionality via the `statx` syscall.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC___SUPPORT_OSUTIL_LINUX_STAT_STAT_VIA_STATX_H
#define LLVM_LIBC_SRC___SUPPORT_OSUTIL_LINUX_STAT_STAT_VIA_STATX_H

#include "hdr/errno_macros.h"
#include "hdr/stdint_proxy.h"
#include "hdr/types/struct_stat.h"
#include "src/__support/OSUtil/linux/stat/kernel_statx_types.h"
#include "src/__support/OSUtil/linux/syscall.h"
#include "src/__support/OSUtil/linux/syscall_wrappers/statx.h"
#include "src/__support/common.h"
#include "src/__support/error_or.h"
#include "src/__support/macros/config.h"

#include <sys/syscall.h> // For syscall numbers

namespace LIBC_NAMESPACE_DECL {
namespace internal {

// A device number as makedev encodes it, which is also how the kernel
// reports one to user space. The kernel header's MKDEV is only right for a
// minor number below 256.
LIBC_INLINE dev_t encode_dev(uint32_t major, uint32_t minor) {
  uint64_t ma = major;
  uint64_t mi = minor;
  return static_cast<dev_t>(((ma & 0x00000fff) << 8) |
                            ((ma & 0xfffff000) << 32) | (mi & 0x000000ff) |
                            ((mi & 0xffffff00) << 12));
}

#ifdef SYS_newfstatat
// The struct stat newfstatat fills in, as the kernel lays it out.
struct kernel_stat {
#ifdef __x86_64__
  unsigned long st_dev;
  unsigned long st_ino;
  unsigned long st_nlink;
  unsigned int st_mode;
  unsigned int st_uid;
  unsigned int st_gid;
  unsigned int pad0;
  unsigned long st_rdev;
  long st_size;
  long st_blksize;
  long st_blocks;
#else
  unsigned long st_dev;
  unsigned long st_ino;
  unsigned int st_mode;
  unsigned int st_nlink;
  unsigned int st_uid;
  unsigned int st_gid;
  unsigned long st_rdev;
  unsigned long pad1;
  long st_size;
  int st_blksize;
  int pad2;
  long st_blocks;
#endif
  long st_atime_sec;
  unsigned long st_atime_nsec;
  long st_mtime_sec;
  unsigned long st_mtime_nsec;
  long st_ctime_sec;
  unsigned long st_ctime_nsec;
  unsigned int unused[3 * sizeof(long) / sizeof(int)];
};

// statx is refused by seccomp filters written before it, which let the older
// stat calls through; the content processes of Firefox are one. Those filters
// answer ENOSYS or EPERM, and newfstatat is asked the same thing instead.
LIBC_INLINE ErrorOr<void> stat_via_newfstatat(int dirfd,
                                              const char *__restrict path,
                                              int flags,
                                              struct stat *__restrict statbuf) {
  kernel_stat kst;
  int ret = syscall_impl<int>(SYS_newfstatat, dirfd, path, &kst, flags);
  if (ret < 0)
    return Error(-ret);

  *statbuf = {};
  // The kernel reports device numbers in the encoding makedev uses.
  statbuf->st_dev = static_cast<dev_t>(kst.st_dev);
  statbuf->st_ino = static_cast<decltype(statbuf->st_ino)>(kst.st_ino);
  statbuf->st_mode = kst.st_mode;
  statbuf->st_nlink = kst.st_nlink;
  statbuf->st_uid = kst.st_uid;
  statbuf->st_gid = kst.st_gid;
  statbuf->st_rdev = static_cast<dev_t>(kst.st_rdev);
  statbuf->st_size = kst.st_size;
  statbuf->st_atim.tv_sec = kst.st_atime_sec;
  statbuf->st_atim.tv_nsec = static_cast<long>(kst.st_atime_nsec);
  statbuf->st_mtim.tv_sec = kst.st_mtime_sec;
  statbuf->st_mtim.tv_nsec = static_cast<long>(kst.st_mtime_nsec);
  statbuf->st_ctim.tv_sec = kst.st_ctime_sec;
  statbuf->st_ctim.tv_nsec = static_cast<long>(kst.st_ctime_nsec);
  statbuf->st_blksize = kst.st_blksize;
  statbuf->st_blocks = kst.st_blocks;
  return {};
}
#endif // SYS_newfstatat

/// Populates `statbuf` via a call to the `statx` syscall.
LIBC_INLINE ErrorOr<void> stat_via_statx(int dirfd, const char *__restrict path,
                                         int flags,
                                         struct stat *__restrict statbuf) {
  kernel_statx_buf xbuf;
  ErrorOr<int> result = linux_syscalls::statx(
      dirfd, path, flags, KERNEL_STATX_BASIC_STATS_MASK, &xbuf);
  if (!result) {
#ifdef SYS_newfstatat
    if (result.error() == ENOSYS || result.error() == EPERM)
      return stat_via_newfstatat(dirfd, path, flags, statbuf);
#endif
    return Error(result.error());
  }

  statbuf->st_dev = encode_dev(xbuf.stx_dev_major, xbuf.stx_dev_minor);
  statbuf->st_ino = static_cast<decltype(statbuf->st_ino)>(xbuf.stx_ino);
  statbuf->st_mode = xbuf.stx_mode;
  statbuf->st_nlink = xbuf.stx_nlink;
  statbuf->st_uid = xbuf.stx_uid;
  statbuf->st_gid = xbuf.stx_gid;
  statbuf->st_rdev = encode_dev(xbuf.stx_rdev_major, xbuf.stx_rdev_minor);
  statbuf->st_size = xbuf.stx_size;
  statbuf->st_atim.tv_sec = xbuf.stx_atime.tv_sec;
  statbuf->st_atim.tv_nsec = xbuf.stx_atime.tv_nsec;
  statbuf->st_mtim.tv_sec = xbuf.stx_mtime.tv_sec;
  statbuf->st_mtim.tv_nsec = xbuf.stx_mtime.tv_nsec;
  statbuf->st_ctim.tv_sec = xbuf.stx_ctime.tv_sec;
  statbuf->st_ctim.tv_nsec = xbuf.stx_ctime.tv_nsec;
  statbuf->st_blksize = xbuf.stx_blksize;
  statbuf->st_blocks =
      static_cast<decltype(statbuf->st_blocks)>(xbuf.stx_blocks);

  return {};
}

} // namespace internal
} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC___SUPPORT_OSUTIL_LINUX_STAT_STAT_VIA_STATX_H
