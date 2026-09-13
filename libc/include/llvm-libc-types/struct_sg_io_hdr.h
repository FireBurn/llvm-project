//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Definition of struct sg_io_hdr.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_TYPES_STRUCT_SG_IO_HDR_H
#define LLVM_LIBC_TYPES_STRUCT_SG_IO_HDR_H

// A command for the SG_IO request, or for write and read on a generic device
// node, and what came back.
typedef struct sg_io_hdr {
  int interface_id;    // 'S'.
  int dxfer_direction; // One of the SG_DXFER_ values.
  unsigned char cmd_len;
  unsigned char mx_sb_len;    // How much of the sense data sbp has room for.
  unsigned short iovec_count; // Nonzero if dxferp is an array of sg_iovec.
  unsigned int dxfer_len;
  void *dxferp;
  unsigned char *cmdp;
  unsigned char *sbp;
  unsigned int timeout; // In milliseconds.
  unsigned int flags;
  int pack_id;
  void *usr_ptr;
  unsigned char status;
  unsigned char masked_status;
  unsigned char msg_status;
  unsigned char sb_len_wr; // How much sense data was written to sbp.
  unsigned short host_status;
  unsigned short driver_status;
  int resid;             // How much of dxfer_len was not transferred.
  unsigned int duration; // In milliseconds.
  unsigned int info;     // SG_INFO_ bits.
} sg_io_hdr_t;

typedef struct sg_io_hdr Sg_io_hdr;

#endif // LLVM_LIBC_TYPES_STRUCT_SG_IO_HDR_H
