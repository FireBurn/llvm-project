//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Definition of type FTS.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_TYPES_FTS_H
#define LLVM_LIBC_TYPES_FTS_H

#include "FTSENT.h"
#include "__fts_compare_func_t.h"
#include "dev_t.h"
#include "size_t.h"

typedef struct {
  FTSENT *fts_cur;   // The entry last returned.
  FTSENT *fts_child; // The entries below it, once fts_children has read them.
  FTSENT **fts_array;
  dev_t fts_dev; // The device the walk started on.
  char *fts_path;
  int fts_rfd;
  size_t fts_pathlen;
  int fts_nitems;
  __fts_compare_func_t fts_compar;
  int fts_options;
} FTS;

#endif // LLVM_LIBC_TYPES_FTS_H
