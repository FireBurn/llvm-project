//===-- Definition of struct fanotify_event_metadata ----------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_TYPES_STRUCT_FANOTIFY_EVENT_METADATA_H
#define LLVM_LIBC_TYPES_STRUCT_FANOTIFY_EVENT_METADATA_H

#include "../llvm-libc-macros/stdint-macros.h"

// One event read from a fanotify group. event_len covers this structure and
// any info records which follow it, so the next event begins event_len bytes
// after the start of this one.
struct fanotify_event_metadata {
  uint32_t event_len;
  uint8_t vers; // FANOTIFY_METADATA_VERSION when the layout is understood.
  uint8_t reserved;
  uint16_t metadata_len; // How much of event_len this structure took.
  __extension__ uint64_t __attribute__((aligned(8))) mask;
  int32_t fd;  // The file it happened to, or FAN_NOFD.
  int32_t pid; // Who caused it, or a pidfd under FAN_REPORT_PIDFD.
};

#endif // LLVM_LIBC_TYPES_STRUCT_FANOTIFY_EVENT_METADATA_H
