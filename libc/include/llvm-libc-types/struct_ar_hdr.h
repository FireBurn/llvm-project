//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_TYPES_STRUCT_AR_HDR_H
#define LLVM_LIBC_TYPES_STRUCT_AR_HDR_H

/// The header in front of each member of an archive.
///
/// Every field is text rather than a number, padded with blanks, and none of
/// them is terminated, which is why the whole thing is a fixed size and why
/// reading one means copying a field out before making sense of it.
struct ar_hdr {
  /// The member's name, sometimes ended with a slash.
  char ar_name[16];
  /// When it was last changed, in seconds since the epoch, written out in
  /// decimal.
  char ar_date[12];
  /// Who owns it, in decimal.
  char ar_uid[6], ar_gid[6];
  /// Its mode, in octal.
  char ar_mode[8];
  /// How many bytes of it follow this header, in decimal.
  char ar_size[10];
  /// Always ARFMAG, which is what says a header was found where one was
  /// expected.
  char ar_fmag[2];
};

#endif // LLVM_LIBC_TYPES_STRUCT_AR_HDR_H
