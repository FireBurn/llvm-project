//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Definition of android_dlextinfo.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_TYPES_ANDROID_DLEXTINFO_H
#define LLVM_LIBC_TYPES_ANDROID_DLEXTINFO_H

#include "../llvm-libc-macros/stdint-macros.h"
#include "off64_t.h"
#include "size_t.h"
#include "struct_android_namespace_t.h"

typedef struct {
  uint64_t flags;
  void *reserved_addr;
  size_t reserved_size;
  int relro_fd;
  int library_fd;
  off64_t library_fd_offset;
  struct android_namespace_t *library_namespace;
} android_dlextinfo;

#endif // LLVM_LIBC_TYPES_ANDROID_DLEXTINFO_H
