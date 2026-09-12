//===-- The Simplified Chinese sets of iconv --------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// GB 2312 by itself, EUC-CN, GBK, GB18030 and ISO-IR-165. They share GB18030's
/// table of two byte codes: GB 2312 and GBK are the parts of it they have.
/// |used| is how many bytes a character took, or for input which is not a
/// character, how many are skipped, which follows glibc.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC_ICONV_CHINESE_H
#define LLVM_LIBC_SRC_ICONV_CHINESE_H

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

// GB 2312: rows and columns from 0x21 to 0x7E.
LIBC_INLINE bool gb2312_assigned(unsigned row, unsigned column) {
  const unsigned r = row - 0x21;
  const unsigned c = column - 0x21;
  if (r >= 94 || c >= 94)
    return false;
  const unsigned bit = r * 94 + c;
  return GB2312_ASSIGNED[bit >> 3] & (1 << (bit & 7));
}

LIBC_INLINE char32_t gb2312_character(unsigned row, unsigned column) {
  if (!gb2312_assigned(row, column))
    return 0;
  const unsigned code = row << 8 | column;
  for (size_t i = 0; i < GB2312_CHANGE_COUNT; ++i)
    if (GB2312_CHANGES[i].code == code)
      return GB2312_CHANGES[i].code_point;
  return look_up(GB18030_TWO_BYTE, row | 0x80, column | 0x80);
}

// The GB 2312 code of |cp|, with both bytes below 0x80, or 0.
LIBC_INLINE uint16_t gb2312_code(char32_t cp) {
  for (size_t i = 0; i < GB2312_CHANGE_COUNT; ++i)
    if (GB2312_CHANGES[i].code_point == cp)
      return GB2312_CHANGES[i].code;
  uint16_t code = find_code(GB18030_TWO_BYTE, cp);
  if ((code >> 8) < 0xA1 || (code & 0xFF) < 0xA1)
    return 0;
  code &= 0x7F7F;
  if (!gb2312_assigned(code >> 8, code & 0xFF))
    return 0;
  for (size_t i = 0; i < GB2312_CHANGE_COUNT; ++i)
    if (GB2312_CHANGES[i].code == code)
      return 0;
  return code;
}

LIBC_INLINE Status read_gb2312(const unsigned char *in, size_t inleft,
                               char32_t &out, size_t &used) {
  used = 1;
  bool row = false;
  for (unsigned column = 0x21; column < 0x7F && !row; ++column)
    row = gb2312_assigned(in[0], column);
  if (!row)
    return Status::INVALID;
  if (inleft < 2)
    return Status::INCOMPLETE;
  char32_t cp = gb2312_character(in[0], in[1]);
  if (cp == 0)
    return Status::INVALID;
  out = cp;
  used = 2;
  return Status::OK;
}

LIBC_INLINE Status write_gb2312(char32_t cp, unsigned char *out, size_t outleft,
                                size_t &made) {
  uint16_t code = gb2312_code(cp);
  if (code == 0)
    return Status::INVALID;
  return put_code(code, 2, out, outleft, made);
}

// ISO-IR-165: GB 2312 with the additions of GB 6345.1 and GB 8565.2, and GB
// 1988 in row 0x2A. glibc has it only inside ISO-2022-CN-EXT, so by itself it
// is read and written as GNU libiconv does: every byte from 0x21 to 0x7E waits
// for a second, and a pair which is not a character is skipped by its first
// byte.
LIBC_INLINE char32_t iso_ir_165_character(unsigned row, unsigned column) {
  if (char32_t cp = gb2312_character(row, column))
    return cp;
  uint16_t cp = 0;
  return look_up(ISO_IR_165_ADDITIONS, row << 8 | column, cp) ? cp : 0;
}

LIBC_INLINE uint16_t iso_ir_165_code(char32_t cp) {
  if (uint16_t code = gb2312_code(cp))
    return code;
  return cp > 0xFFFF ? 0 : find_code(ISO_IR_165_ADDITIONS, cp);
}

LIBC_INLINE Status read_iso_ir_165(const unsigned char *in, size_t inleft,
                                   char32_t &out, size_t &used) {
  used = 1;
  if (in[0] < 0x21 || in[0] > 0x7E)
    return Status::INVALID;
  if (inleft < 2)
    return Status::INCOMPLETE;
  const char32_t cp = iso_ir_165_character(in[0], in[1]);
  if (cp == 0)
    return Status::INVALID;
  used = 2;
  out = cp;
  return Status::OK;
}

LIBC_INLINE Status write_iso_ir_165(char32_t cp, unsigned char *out,
                                    size_t outleft, size_t &made) {
  const uint16_t code = iso_ir_165_code(cp);
  if (code == 0)
    return Status::INVALID;
  return put_code(code, 2, out, outleft, made);
}

// EUC-CN: ASCII, and GB 2312 with both bytes from 0xA1. As in glibc, 0x8E and
// 0x8F wait for a second byte, though no code begins with them.
LIBC_INLINE Status read_euc_cn(const unsigned char *in, size_t inleft,
                               char32_t &out, size_t &used) {
  const unsigned lead = in[0];
  used = 1;
  if (lead < 0x80) {
    out = lead;
    return Status::OK;
  }
  if (lead != 0x8E && lead != 0x8F && (lead < 0xA1 || lead > 0xFE))
    return Status::INVALID;
  if (inleft < 2)
    return Status::INCOMPLETE;
  const unsigned trail = in[1];
  if (trail < 0xA1)
    return Status::INVALID;
  used = 2;
  char32_t cp = lead >= 0xA1 && trail < 0xFF
                    ? gb2312_character(lead - 0x80, trail - 0x80)
                    : 0;
  if (cp == 0)
    return Status::INVALID;
  out = cp;
  return Status::OK;
}

LIBC_INLINE Status write_euc_cn(char32_t cp, unsigned char *out, size_t outleft,
                                size_t &made) {
  if (cp < 0x80)
    return put_code(cp, 1, out, outleft, made);
  uint16_t code = gb2312_code(cp);
  if (code == 0)
    return Status::INVALID;
  return put_code(code | 0x8080, 2, out, outleft, made);
}

// GBK, which glibc also calls CP936: GB18030's two byte codes without its
// private use characters and later additions, and the euro sign at 0x80.
LIBC_INLINE bool gbk_has(unsigned code, char32_t cp) {
  if (cp == 0 || (cp >= 0xE000 && cp <= 0xF8FF))
    return false;
  for (size_t i = 0; i < GBK_EXCLUDED_COUNT; ++i)
    if (code >= GBK_EXCLUDED[i].first && code <= GBK_EXCLUDED[i].last)
      return false;
  return true;
}

LIBC_INLINE Status read_gbk(const unsigned char *in, size_t inleft,
                            char32_t &out, size_t &used) {
  const unsigned lead = in[0];
  used = 1;
  if (lead < 0x80) {
    out = lead;
    return Status::OK;
  }
  if (lead == 0x80) {
    out = 0x20AC;
    return Status::OK;
  }
  if (lead == 0xFF)
    return Status::INVALID;
  if (inleft < 2)
    return Status::INCOMPLETE;
  const unsigned trail = in[1];
  if (trail < 0x40)
    return Status::INVALID;
  char32_t cp = look_up(GB18030_TWO_BYTE, lead, trail);
  if (!gbk_has(lead << 8 | trail, cp)) {
    // glibc skips only the 0xFE of a code in GB18030's user-defined row
    // from 0xFEA1.
    if (lead != 0xFE || trail < 0xA1)
      used = 2;
    return Status::INVALID;
  }
  used = 2;
  out = cp;
  return Status::OK;
}

LIBC_INLINE Status write_gbk(char32_t cp, unsigned char *out, size_t outleft,
                             size_t &made) {
  if (cp < 0x80)
    return put_code(cp, 1, out, outleft, made);
  if (cp == 0x20AC)
    return put_code(0x80, 1, out, outleft, made);
  uint16_t code = find_code(GB18030_TWO_BYTE, cp);
  if (code == 0 || !gbk_has(code, cp))
    return Status::INVALID;
  return put_code(code, 2, out, outleft, made);
}

// GB18030: GBK's layout with every code point of Unicode. What has no two byte
// code has a four byte one, a byte from 0x81, a digit, a byte from 0x81 and a
// digit. Those of the Basic Multilingual Plane come in runs, and the rest are
// in order from 0x90308130.
constexpr uint32_t GB18030_SUPPLEMENTARY_START = 189000;

LIBC_INLINE char32_t gb18030_four_byte_character(uint32_t position) {
  if (position < GB18030_FOUR_BYTE_COUNT) {
    size_t low = 0;
    size_t high = GB18030_RUN_COUNT;
    while (high - low > 1) {
      size_t mid = low + (high - low) / 2;
      if (GB18030_RUNS[mid].position <= position)
        low = mid;
      else
        high = mid;
    }
    return GB18030_RUNS[low].code_point +
           (position - GB18030_RUNS[low].position);
  }
  if (position >= GB18030_SUPPLEMENTARY_START &&
      position - GB18030_SUPPLEMENTARY_START < 0x100000)
    return 0x10000 + (position - GB18030_SUPPLEMENTARY_START);
  return 0;
}

// The position of the four byte code of a code point with no two byte code.
LIBC_INLINE uint32_t gb18030_four_byte_position(char32_t cp) {
  if (cp >= 0x10000)
    return GB18030_SUPPLEMENTARY_START + (cp - 0x10000);
  size_t low = 0;
  size_t high = GB18030_RUN_COUNT;
  while (high - low > 1) {
    size_t mid = low + (high - low) / 2;
    if (GB18030_RUNS[GB18030_RUNS_BY_CODE_POINT[mid]].code_point <= cp)
      low = mid;
    else
      high = mid;
  }
  const size_t run = GB18030_RUNS_BY_CODE_POINT[low];
  return GB18030_RUNS[run].position + (cp - GB18030_RUNS[run].code_point);
}

LIBC_INLINE Status read_gb18030(const unsigned char *in, size_t inleft,
                                char32_t &out, size_t &used) {
  const unsigned lead = in[0];
  used = 1;
  if (lead < 0x80) {
    out = lead;
    return Status::OK;
  }
  if (lead == 0x80 || lead == 0xFF)
    return Status::INVALID;
  if (inleft < 2)
    return Status::INCOMPLETE;
  const unsigned second = in[1];
  if (second >= 0x30 && second <= 0x39) {
    // Four bytes are waited for, whatever the third is.
    if (inleft < 4)
      return Status::INCOMPLETE;
    const unsigned third = in[2];
    const unsigned fourth = in[3];
    used = 2;
    if (third < 0x81 || third > 0xFE)
      return Status::INVALID;
    used = 4;
    if (fourth < 0x30 || fourth > 0x39)
      return Status::INVALID;
    const uint32_t position =
        (((lead - 0x81) * 10 + second - 0x30) * 126 + third - 0x81) * 10 +
        fourth - 0x30;
    char32_t cp = gb18030_four_byte_character(position);
    if (cp == 0)
      return Status::INVALID;
    out = cp;
    return Status::OK;
  }
  used = 2;
  char32_t cp = look_up(GB18030_TWO_BYTE, lead, second);
  if (cp == 0)
    return Status::INVALID;
  out = cp;
  return Status::OK;
}

LIBC_INLINE Status write_gb18030(char32_t cp, unsigned char *out,
                                 size_t outleft, size_t &made) {
  if (cp < 0x80)
    return put_code(cp, 1, out, outleft, made);
  if (uint16_t code = find_code(GB18030_TWO_BYTE, cp))
    return put_code(code, 2, out, outleft, made);
  if ((cp >= 0xD800 && cp <= 0xDFFF) || cp > 0x10FFFF)
    return Status::INVALID;
  uint32_t position = gb18030_four_byte_position(cp);
  const unsigned fourth = 0x30 + position % 10;
  position /= 10;
  const unsigned third = 0x81 + position % 126;
  position /= 126;
  const unsigned second = 0x30 + position % 10;
  const unsigned first = 0x81 + position / 10;
  return put_code(first << 24 | second << 16 | third << 8 | fourth, 4, out,
                  outleft, made);
}

} // namespace iconv_internal
} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC_ICONV_CHINESE_H
