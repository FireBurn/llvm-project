//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Definition of struct statx and struct statx_timestamp.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_TYPES_STRUCT_STATX_H
#define LLVM_LIBC_TYPES_STRUCT_STATX_H

#include "../llvm-libc-macros/stdint-macros.h"

// A time as statx reports it, which unlike struct timespec is the same
// width everywhere.
struct statx_timestamp {
  int64_t tv_sec;
  uint32_t tv_nsec;
  int32_t __reserved;
};

// What statx fills in. The kernel writes only the fields the caller asked
// for and says in stx_mask which it wrote, so a field whose bit is not in
// the mask holds nothing worth reading. The spare space at the end is the
// kernel's room to add fields without changing the size, which is fixed at
// 256 bytes.
struct statx {
  uint32_t stx_mask;
  uint32_t stx_blksize;
  uint64_t stx_attributes;

  uint32_t stx_nlink;
  uint32_t stx_uid;
  uint32_t stx_gid;
  uint16_t stx_mode;
  uint16_t __spare0[1];

  uint64_t stx_ino;
  uint64_t stx_size;
  uint64_t stx_blocks;
  uint64_t stx_attributes_mask;

  struct statx_timestamp stx_atime;
  struct statx_timestamp stx_btime;
  struct statx_timestamp stx_ctime;
  struct statx_timestamp stx_mtime;

  // The device a special file names, and the one the file is on. These are
  // the two halves of a device number rather than a dev_t.
  uint32_t stx_rdev_major;
  uint32_t stx_rdev_minor;
  uint32_t stx_dev_major;
  uint32_t stx_dev_minor;

  uint64_t stx_mnt_id;
  uint32_t stx_dio_mem_align;
  uint32_t stx_dio_offset_align;

  uint64_t stx_subvol;
  uint32_t stx_atomic_write_unit_min;
  uint32_t stx_atomic_write_unit_max;

  uint32_t stx_atomic_write_segments_max;
  uint32_t stx_dio_read_offset_align;
  uint32_t stx_atomic_write_unit_max_opt;
  uint32_t __spare2[1];

  uint64_t __spare3[8];
};

#endif // LLVM_LIBC_TYPES_STRUCT_STATX_H
