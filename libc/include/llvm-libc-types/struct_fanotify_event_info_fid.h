//===-- Definition of struct fanotify_event_info_fid ----------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_TYPES_STRUCT_FANOTIFY_EVENT_INFO_FID_H
#define LLVM_LIBC_TYPES_STRUCT_FANOTIFY_EVENT_INFO_FID_H

#include "fsid_t.h"
#include "struct_fanotify_event_info_header.h"

// The FID, DFID and DFID_NAME records. What follows the fsid is a file
// handle of the shape open_by_handle_at takes, and for DFID_NAME a null
// terminated name after that.
struct fanotify_event_info_fid {
  struct fanotify_event_info_header hdr;
  fsid_t fsid;
  unsigned char handle[];
};

#endif // LLVM_LIBC_TYPES_STRUCT_FANOTIFY_EVENT_INFO_FID_H
