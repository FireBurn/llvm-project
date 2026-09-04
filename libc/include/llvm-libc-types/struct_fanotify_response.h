//===-- Definition of struct fanotify_response ----------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_TYPES_STRUCT_FANOTIFY_RESPONSE_H
#define LLVM_LIBC_TYPES_STRUCT_FANOTIFY_RESPONSE_H

#include "../llvm-libc-macros/stdint-macros.h"

// What is written back to the group to answer a permission event. The fd is
// the one the event carried.
struct fanotify_response {
  int32_t fd;
  uint32_t response;
};

// An answer which carries FAN_INFO is written as this instead, with the
// record following it.
struct fanotify_response_info_header {
  uint8_t type;
  uint8_t pad;
  uint16_t len;
};

struct fanotify_response_info_audit_rule {
  struct fanotify_response_info_header hdr;
  uint32_t rule_number;
  uint32_t subj_trust;
  uint32_t obj_trust;
};

#endif // LLVM_LIBC_TYPES_STRUCT_FANOTIFY_RESPONSE_H
