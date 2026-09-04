//===-- Definition of struct fanotify_event_info_pidfd --------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_TYPES_STRUCT_FANOTIFY_EVENT_INFO_PIDFD_H
#define LLVM_LIBC_TYPES_STRUCT_FANOTIFY_EVENT_INFO_PIDFD_H

#include "../llvm-libc-macros/stdint-macros.h"
#include "struct_fanotify_event_info_header.h"

// A descriptor for the process which caused the event, which does not go
// stale the way its pid would.
struct fanotify_event_info_pidfd {
  struct fanotify_event_info_header hdr;
  int32_t pidfd;
};

#endif // LLVM_LIBC_TYPES_STRUCT_FANOTIFY_EVENT_INFO_PIDFD_H
