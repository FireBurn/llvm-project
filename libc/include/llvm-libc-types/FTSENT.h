//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Definition of type FTSENT.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_TYPES_FTSENT_H
#define LLVM_LIBC_TYPES_FTSENT_H

#include "dev_t.h"
#include "ino_t.h"
#include "nlink_t.h"
#include "size_t.h"
#include "struct_stat.h"

typedef struct _ftsent {
  struct _ftsent *fts_cycle;  // The ancestor a directory loops back to.
  struct _ftsent *fts_parent; // The directory this is in.
  struct _ftsent *fts_link;   // The next entry in the directory.
  long fts_number;            // For the caller's use.
  void *fts_pointer;          // For the caller's use.
  char *fts_accpath;          // The path to reach the entry by.
  char *fts_path;             // The path from the root.
  int fts_errno;              // errno for this entry.
  int fts_symfd;              // Unused.
  size_t fts_pathlen;
  size_t fts_namelen;
  ino_t fts_ino;
  dev_t fts_dev;
  nlink_t fts_nlink;
  short fts_level; // How far below the root, which is 0.
  unsigned short fts_info;
  unsigned short fts_flags;
  unsigned short fts_instr;
  struct stat *fts_statp;
  char fts_name[1];
} FTSENT;

#endif // LLVM_LIBC_TYPES_FTSENT_H
