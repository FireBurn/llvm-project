//===-- The Traditional Chinese sets of iconv -------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// BIG5, which glibc also calls CP950. |used| is how many bytes a character
/// took, or for input which is not a character, how many are skipped, which
/// follows glibc.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC_ICONV_TRADITIONAL_CHINESE_H
#define LLVM_LIBC_SRC_ICONV_TRADITIONAL_CHINESE_H

#include "hdr/stdint_proxy.h"
#include "hdr/types/char32_t.h"
#include "hdr/types/size_t.h"
#include "src/__support/macros/attributes.h"
#include "src/__support/macros/config.h"
#include "src/iconv/cjk_tables.h"
#include "src/iconv/code_table.h"
#include "src/iconv/japanese.h"
#include "src/iconv/status.h"

namespace LIBC_NAMESPACE_DECL {
namespace iconv_internal {

// BIG5: ASCII and 0x80 by themselves, and a lead byte from 0xA1 to 0xF9 with a
// trail byte from 0x40 to 0x7E or from 0xA1 to 0xFE.
LIBC_INLINE Status read_big5(const unsigned char *in, size_t inleft,
                             char32_t &out, size_t &used) {
  const unsigned lead = in[0];
  used = 1;
  if (lead <= 0x80) {
    out = lead;
    return Status::OK;
  }
  if (lead < 0xA1 || lead > 0xF9)
    return Status::INVALID;
  if (inleft < 2)
    return Status::INCOMPLETE;
  const unsigned trail = in[1];
  if (trail < 0x40 || (trail > 0x7E && trail < 0xA1) || trail == 0xFF)
    return Status::INVALID;
  // As in glibc, a pair of lead and trail bytes with no character is skipped
  // whole.
  used = 2;
  const char32_t cp = look_up(BIG5_TWO_BYTE, lead, trail);
  if (cp == 0)
    return Status::INVALID;
  out = cp;
  return Status::OK;
}

LIBC_INLINE Status write_big5(char32_t cp, unsigned char *out, size_t outleft,
                              size_t &made) {
  if (cp <= 0x80)
    return put_code(cp, 1, out, outleft, made);
  const uint16_t code = find_code(BIG5_TWO_BYTE, cp);
  if (code == 0)
    return Status::INVALID;
  return put_code(code, 2, out, outleft, made);
}

} // namespace iconv_internal
} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC_ICONV_TRADITIONAL_CHINESE_H
