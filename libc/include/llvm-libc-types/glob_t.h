//===-- Definition of type glob_t -----------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_TYPES_GLOB_T_H
#define LLVM_LIBC_TYPES_GLOB_T_H

#include "size_t.h"
#include "struct_dirent.h"
#include "struct_stat.h"

typedef struct {
  size_t gl_pathc; // The number of paths matched.
  char **gl_pathv; // The matched paths, followed by a null pointer.
  size_t gl_offs;  // Slots to leave empty at the front of gl_pathv.

  // What the search did, which GLOB_MAGCHAR is set in when the pattern held
  // a metacharacter.
  int gl_flags;

  // Where the search reads from, when GLOB_ALTDIRFUNC says to use these
  // rather than the ordinary calls. That is how a caller searches
  // something which is not the filesystem, or the filesystem seen through
  // its own descriptors.
  void (*gl_closedir)(void *);
  struct dirent *(*gl_readdir)(void *);
  void *(*gl_opendir)(const char *);
  int (*gl_lstat)(const char *__restrict, struct stat *__restrict);
  int (*gl_stat)(const char *__restrict, struct stat *__restrict);
} glob_t;

#endif // LLVM_LIBC_TYPES_GLOB_T_H
