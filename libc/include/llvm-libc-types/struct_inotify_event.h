//===-- Definition of struct inotify_event --------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_TYPES_STRUCT_INOTIFY_EVENT_H
#define LLVM_LIBC_TYPES_STRUCT_INOTIFY_EVENT_H

#include "../llvm-libc-macros/stdint-macros.h"

// One event read from an inotify descriptor. The name follows the structure
// rather than being pointed at, and len says how much room it took, so the
// next event begins len bytes after the end of this one.
struct inotify_event {
  int wd;          // Which watch it came from.
  uint32_t mask;   // What happened.
  uint32_t cookie; // Pairs a move out with the move in which matches it.
  uint32_t len;    // How much room the name took, padding included.
  char name[];
};

#endif // LLVM_LIBC_TYPES_STRUCT_INOTIFY_EVENT_H
