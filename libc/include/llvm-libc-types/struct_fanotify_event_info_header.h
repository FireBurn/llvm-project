//===-- Definition of struct fanotify_event_info_header -------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_TYPES_STRUCT_FANOTIFY_EVENT_INFO_HEADER_H
#define LLVM_LIBC_TYPES_STRUCT_FANOTIFY_EVENT_INFO_HEADER_H

#include "../llvm-libc-macros/stdint-macros.h"

// What every info record following an event begins with. info_type says
// which of the records below it is, and len how much room it took.
struct fanotify_event_info_header {
  uint8_t info_type;
  uint8_t pad;
  uint16_t len;
};

#endif // LLVM_LIBC_TYPES_STRUCT_FANOTIFY_EVENT_INFO_HEADER_H
