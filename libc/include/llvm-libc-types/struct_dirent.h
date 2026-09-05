//===-- Definition of type struct dirent ----------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_TYPES_STRUCT_DIRENT_H
#define LLVM_LIBC_TYPES_STRUCT_DIRENT_H

#include "ino_t.h"
#include "off_t.h"
#include "reclen_t.h"

#include "../llvm-libc-macros/limits-macros.h"

struct dirent {
  ino_t d_ino;
#ifdef __linux__
  off_t d_off;
  reclen_t d_reclen;
#endif
  unsigned char d_type;
  // How long the name of an entry readdir returns actually is has to be found
  // with strlen, or on Linux from d_reclen: what readdir hands back points
  // into a buffer of packed entries, each only as long as its own name needs,
  // so reading the whole array from one of those reads past it.
  //
  // The array is nonetheless as long as a name may be, because a program is
  // allowed to declare one of these itself and put a name in it, which is
  // what every other libc lets it do.
  char d_name[NAME_MAX + 1];
};

#endif // LLVM_LIBC_TYPES_STRUCT_DIRENT_H
