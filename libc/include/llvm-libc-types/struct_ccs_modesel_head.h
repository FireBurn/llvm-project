//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Definition of struct ccs_modesel_head.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_TYPES_STRUCT_CCS_MODESEL_HEAD_H
#define LLVM_LIBC_TYPES_STRUCT_CCS_MODESEL_HEAD_H

// The header of MODE SELECT and MODE SENSE data, followed by one block
// descriptor, with the multibyte numbers most significant byte first.
struct ccs_modesel_head {
  unsigned char _r1;
  unsigned char medium;
  unsigned char _r2;
  unsigned char block_desc_length;
  unsigned char density;
  unsigned char number_blocks_hi;
  unsigned char number_blocks_med;
  unsigned char number_blocks_lo;
  unsigned char _r3;
  unsigned char block_length_hi;
  unsigned char block_length_med;
  unsigned char block_length_lo;
};

#endif // LLVM_LIBC_TYPES_STRUCT_CCS_MODESEL_HEAD_H
