//===-- Definition of struct fanotify_event_info_range --------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_TYPES_STRUCT_FANOTIFY_EVENT_INFO_RANGE_H
#define LLVM_LIBC_TYPES_STRUCT_FANOTIFY_EVENT_INFO_RANGE_H

#include "../llvm-libc-macros/stdint-macros.h"
#include "struct_fanotify_event_info_header.h"

// Which part of the file a pre-content event wants filled in.
struct fanotify_event_info_range {
  struct fanotify_event_info_header hdr;
  uint32_t pad;
  uint64_t offset;
  uint64_t count;
};

#endif // LLVM_LIBC_TYPES_STRUCT_FANOTIFY_EVENT_INFO_RANGE_H
