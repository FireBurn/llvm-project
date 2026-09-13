//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Definition of struct sg_req_info.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_TYPES_STRUCT_SG_REQ_INFO_H
#define LLVM_LIBC_TYPES_STRUCT_SG_REQ_INFO_H

// One entry of the table SG_GET_REQUEST_TABLE fills.
typedef struct sg_req_info {
  char req_state; // 0 unused, 1 written and waiting, 2 ready to read.
  char orphan;
  char sg_io_owned;
  char problem;
  int pack_id;
  void *usr_ptr;
  unsigned int duration; // In milliseconds.
  int unused;
} sg_req_info_t;

typedef struct sg_req_info Sg_req_info;

#endif // LLVM_LIBC_TYPES_STRUCT_SG_REQ_INFO_H
