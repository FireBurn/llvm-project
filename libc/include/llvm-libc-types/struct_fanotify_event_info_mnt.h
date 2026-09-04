//===-- Definition of struct fanotify_event_info_mnt ----------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_TYPES_STRUCT_FANOTIFY_EVENT_INFO_MNT_H
#define LLVM_LIBC_TYPES_STRUCT_FANOTIFY_EVENT_INFO_MNT_H

#include "../llvm-libc-macros/stdint-macros.h"
#include "struct_fanotify_event_info_header.h"

// Which mount was attached or detached.
struct fanotify_event_info_mnt {
  struct fanotify_event_info_header hdr;
  uint64_t mnt_id;
};

#endif // LLVM_LIBC_TYPES_STRUCT_FANOTIFY_EVENT_INFO_MNT_H
