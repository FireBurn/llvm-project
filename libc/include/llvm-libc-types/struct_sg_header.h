//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Definition of struct sg_header.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_TYPES_STRUCT_SG_HEADER_H
#define LLVM_LIBC_TYPES_STRUCT_SG_HEADER_H

#include "../llvm-libc-macros/scsi-sg-macros.h"

// What precedes a command written to a generic device node, and its reply
// read back, in the interface before sg_io_hdr.
struct sg_header {
  int pack_len;
  int reply_len;
  int pack_id;
  int result; // An errno value.
  unsigned int twelve_byte : 1;
  unsigned int target_status : 5;
  unsigned int host_status : 8;
  unsigned int driver_status : 8;
  unsigned int other_flags : 10;
  unsigned char sense_buffer[SG_MAX_SENSE];
};

#endif // LLVM_LIBC_TYPES_STRUCT_SG_HEADER_H
