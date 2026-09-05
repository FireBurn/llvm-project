//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Definition of struct dqblk and struct dqinfo.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_TYPES_STRUCT_DQBLK_H
#define LLVM_LIBC_TYPES_STRUCT_DQBLK_H

#include "../llvm-libc-macros/stdint-macros.h"

// One user's or group's limits and usage. dqb_valid says which of the rest
// the caller filled in, or which the kernel did.
struct dqblk {
  uint64_t dqb_bhardlimit; // Blocks it may not go past.
  uint64_t dqb_bsoftlimit; // Blocks it should stay under.
  uint64_t dqb_curspace;   // Bytes it is using.
  uint64_t dqb_ihardlimit; // Inodes it may not go past.
  uint64_t dqb_isoftlimit; // Inodes it should stay under.
  uint64_t dqb_curinodes;  // Inodes it is using.
  uint64_t dqb_btime;      // When the block grace period runs out.
  uint64_t dqb_itime;      // When the inode one does.
  uint32_t dqb_valid;
};

// The filesystem's own settings, rather than one user's.
struct dqinfo {
  uint64_t dqi_bgrace;
  uint64_t dqi_igrace;
  uint32_t dqi_flags;
  uint32_t dqi_valid;
};

#endif // LLVM_LIBC_TYPES_STRUCT_DQBLK_H
