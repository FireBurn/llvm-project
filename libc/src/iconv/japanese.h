//===-- The Japanese sets of iconv ------------------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// JIS X 0208 and JIS X 0212 by themselves, and EUC-JP, Shift_JIS and CP932,
/// which are built on them. |used| is how many bytes a character took, or for
/// input which is not a character, how many are skipped, which follows glibc.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC_ICONV_JAPANESE_H
#define LLVM_LIBC_SRC_ICONV_JAPANESE_H

#include "hdr/stdint_proxy.h"
#include "hdr/types/char32_t.h"
#include "hdr/types/size_t.h"
#include "src/__support/macros/attributes.h"
#include "src/__support/macros/config.h"
#include "src/iconv/cjk_tables.h"
#include "src/iconv/code_table.h"
#include "src/iconv/status.h"

namespace LIBC_NAMESPACE_DECL {
namespace iconv_internal {

// Writes a code of |length| bytes, high byte first.
LIBC_INLINE Status put_code(uint32_t code, size_t length, unsigned char *out,
                            size_t outleft, size_t &made) {
  if (outleft < length)
    return Status::FULL;
  for (size_t i = 0; i < length; ++i)
    out[i] = static_cast<unsigned char>(code >> (8 * (length - 1 - i)));
  made = length;
  return Status::OK;
}

// Half-width katakana, which JIS X 0201 has from 0xA1 to 0xDF.
LIBC_INLINE bool is_kana_byte(unsigned byte) {
  return byte >= 0xA1 && byte <= 0xDF;
}

LIBC_INLINE bool row_has_characters(const CodeTable &table, unsigned lead) {
  for (unsigned trail = 0x21; trail < 0x7F; ++trail)
    if (look_up(table, lead, trail) != 0)
      return true;
  return false;
}

// A set of 94 rows of 94 characters by itself, such as JIS X 0208, JIS X 0212
// or KS X 1001: each character is two bytes from 0x21 to 0x7E. A lead byte
// from a row with no characters is not one.
LIBC_INLINE Status read_jis(const CodeTable &table, const unsigned char *in,
                            size_t inleft, char32_t &out, size_t &used) {
  used = 1;
  if (!row_has_characters(table, in[0]))
    return Status::INVALID;
  if (inleft < 2)
    return Status::INCOMPLETE;
  char32_t cp = look_up(table, in[0], in[1]);
  if (cp == 0)
    return Status::INVALID;
  out = cp;
  used = 2;
  return Status::OK;
}

LIBC_INLINE Status write_jis(const CodeTable &table, char32_t cp,
                             unsigned char *out, size_t outleft, size_t &made) {
  uint16_t code = find_code(table, cp);
  if (code == 0)
    return Status::INVALID;
  return put_code(code, 2, out, outleft, made);
}

// EUC-JP: ASCII, JIS X 0208 with both bytes from 0xA1, half-width katakana
// after 0x8E, and JIS X 0212 after 0x8F. As in glibc, the other C1 bytes are
// the C1 controls.
LIBC_INLINE Status read_euc_jp(const unsigned char *in, size_t inleft,
                               char32_t &out, size_t &used) {
  const unsigned lead = in[0];
  used = 1;
  if (lead < 0xA0 && lead != 0x8E && lead != 0x8F) {
    out = lead;
    return Status::OK;
  }
  if (lead == 0xFF)
    return Status::INVALID;
  if (inleft < 2)
    return Status::INCOMPLETE;
  const unsigned second = in[1];
  if (second < 0xA1)
    return Status::INVALID;

  if (lead == 0x8E) {
    used = 2;
    if (!is_kana_byte(second))
      return Status::INVALID;
    out = 0xFF61 + (second - 0xA1);
    return Status::OK;
  }

  if (lead == 0x8F) {
    // A row JIS X 0212 has nothing in from its first or last is rejected as
    // soon as it is seen; an unassigned code only skips the 0x8F.
    if (second == 0xA1 || second > 0xED)
      return Status::INVALID;
    if (inleft < 3)
      return Status::INCOMPLETE;
    char32_t cp = look_up(JIS_X0212, second - 0x80, in[2] - 0x80);
    if (cp == 0 || in[2] < 0xA1)
      return Status::INVALID;
    out = cp;
    used = 3;
    return Status::OK;
  }

  used = 2;
  char32_t cp = look_up(JIS_X0208, lead - 0x80, second - 0x80);
  if (cp == 0)
    return Status::INVALID;
  out = cp;
  return Status::OK;
}

LIBC_INLINE Status write_euc_jp(char32_t cp, unsigned char *out, size_t outleft,
                                size_t &made) {
  if (cp < 0xA0 && cp != 0x8E && cp != 0x8F)
    return put_code(cp, 1, out, outleft, made);
  // glibc writes JIS X 0201's yen sign and overline as the ASCII they replace.
  if (cp == 0xA5)
    return put_code(0x5C, 1, out, outleft, made);
  if (cp == 0x203E)
    return put_code(0x7E, 1, out, outleft, made);
  if (cp >= 0xFF61 && cp <= 0xFF9F)
    return put_code(0x8EA1 + (cp - 0xFF61), 2, out, outleft, made);
  if (uint16_t code = find_code(JIS_X0208, cp))
    return put_code(code | 0x8080, 2, out, outleft, made);
  if (uint16_t code = find_code(JIS_X0212, cp))
    return put_code(0x8F8080 | code, 3, out, outleft, made);
  return Status::INVALID;
}

// The JIS X 0208 code of a Shift_JIS lead and trail byte.
LIBC_INLINE uint16_t jis_from_shift_jis(unsigned lead, unsigned trail) {
  unsigned row = 2 * (lead < 0xA0 ? lead - 0x81 : lead - 0xC1);
  unsigned column;
  if (trail >= 0x9F) {
    ++row;
    column = trail - 0x9F;
  } else {
    column = trail - 0x40 - (trail > 0x7F ? 1 : 0);
  }
  return static_cast<uint16_t>((0x21 + row) << 8 | (0x21 + column));
}

LIBC_INLINE uint16_t shift_jis_from_jis(uint16_t code) {
  const unsigned row = (code >> 8) - 0x21;
  const unsigned column = (code & 0xFF) - 0x21;
  const unsigned lead = (row >> 1) + (row < 62 ? 0x81 : 0xC1);
  const unsigned trail =
      (row & 1) ? column + 0x9F : column + 0x40 + (column >= 0x3F ? 1 : 0);
  return static_cast<uint16_t>(lead << 8 | trail);
}

// Shift_JIS: JIS X 0201, with its yen sign and overline in place of ASCII's
// reverse solidus and tilde, and JIS X 0208 rearranged into two bytes.
LIBC_INLINE Status read_shift_jis(const unsigned char *in, size_t inleft,
                                  char32_t &out, size_t &used) {
  const unsigned lead = in[0];
  used = 1;
  if (lead < 0x80) {
    out = lead == 0x5C ? 0xA5 : lead == 0x7E ? 0x203E : lead;
    return Status::OK;
  }
  if (is_kana_byte(lead)) {
    out = 0xFF61 + (lead - 0xA1);
    return Status::OK;
  }
  if (!(lead >= 0x81 && lead <= 0x9F) && !(lead >= 0xE0 && lead <= 0xEA))
    return Status::INVALID;
  if (inleft < 2)
    return Status::INCOMPLETE;
  const unsigned trail = in[1];
  if (trail < 0x40)
    return Status::INVALID;
  used = 2;
  if (trail == 0x7F || trail > 0xFC)
    return Status::INVALID;
  const uint16_t code = jis_from_shift_jis(lead, trail);
  char32_t cp = look_up(JIS_X0208, code >> 8, code & 0xFF);
  if (cp == 0)
    return Status::INVALID;
  out = cp;
  return Status::OK;
}

LIBC_INLINE Status write_shift_jis(char32_t cp, unsigned char *out,
                                   size_t outleft, size_t &made) {
  // glibc writes ASCII's reverse solidus and tilde as the bytes they share
  // with the yen sign and overline.
  if (cp < 0x80)
    return put_code(cp, 1, out, outleft, made);
  if (cp == 0xA5)
    return put_code(0x5C, 1, out, outleft, made);
  if (cp == 0x203E)
    return put_code(0x7E, 1, out, outleft, made);
  if (cp >= 0xFF61 && cp <= 0xFF9F)
    return put_code(0xA1 + (cp - 0xFF61), 1, out, outleft, made);
  // And the fullwidth cent, pound and not signs, which Microsoft uses, as the
  // JIS X 0208 codes of the plain ones.
  if (cp == 0xFFE0)
    return put_code(0x8191, 2, out, outleft, made);
  if (cp == 0xFFE1)
    return put_code(0x8192, 2, out, outleft, made);
  if (cp == 0xFFE2)
    return put_code(0x81CA, 2, out, outleft, made);
  if (uint16_t code = find_code(JIS_X0208, cp))
    return put_code(shift_jis_from_jis(code), 2, out, outleft, made);
  return Status::INVALID;
}

// CP932: Shift_JIS as Microsoft has it, with ASCII in full, some JIS X 0208
// codes read as other characters, NEC's and IBM's extensions, and the
// user-defined area from 0xF040 to 0xF9FC read as private use characters.
LIBC_INLINE char32_t cp932_character(unsigned lead, unsigned trail) {
  if (lead >= 0xF0 && lead <= 0xF9)
    return 0xE000 + (lead - 0xF0) * 188 + (trail - 0x40) -
           (trail > 0x7F ? 1 : 0);
  const unsigned code = lead << 8 | trail;
  for (size_t i = 0; i < CP932_CHANGE_COUNT; ++i)
    if (CP932_CHANGES[i].code == code)
      return CP932_CHANGES[i].code_point;
  if (lead == 0x87)
    return look_up(CP932_NEC_ROW_13, lead, trail);
  if (lead == 0xED || lead == 0xEE)
    return look_up(CP932_NEC_SELECTED_IBM, lead, trail);
  if (lead >= 0xFA)
    return look_up(CP932_IBM, lead, trail);
  if (lead > 0xEA)
    return 0;
  const uint16_t jis = jis_from_shift_jis(lead, trail);
  return look_up(JIS_X0208, jis >> 8, jis & 0xFF);
}

LIBC_INLINE Status read_cp932(const unsigned char *in, size_t inleft,
                              char32_t &out, size_t &used) {
  const unsigned lead = in[0];
  used = 1;
  if (lead < 0x80) {
    out = lead;
    return Status::OK;
  }
  if (is_kana_byte(lead)) {
    out = 0xFF61 + (lead - 0xA1);
    return Status::OK;
  }
  if (!(lead >= 0x81 && lead <= 0x9F) && !(lead >= 0xE0 && lead <= 0xFC))
    return Status::INVALID;
  if (inleft < 2)
    return Status::INCOMPLETE;
  const unsigned trail = in[1];
  if (trail < 0x40 || trail == 0x7F || trail > 0xFC)
    return Status::INVALID;
  char32_t cp = cp932_character(lead, trail);
  if (cp == 0) {
    // As in glibc, a code inside one of the areas CP932 is laid out in is
    // skipped whole, and one outside them by its lead byte alone.
    const unsigned code = lead << 8 | trail;
    for (size_t i = 0; i < CP932_AREA_COUNT; ++i)
      if (code >= CP932_AREAS[i].first && code <= CP932_AREAS[i].last)
        used = 2;
    return Status::INVALID;
  }
  out = cp;
  used = 2;
  return Status::OK;
}

LIBC_INLINE Status write_cp932(char32_t cp, unsigned char *out, size_t outleft,
                               size_t &made) {
  if (cp < 0x80)
    return put_code(cp, 1, out, outleft, made);
  if (cp >= 0xFF61 && cp <= 0xFF9F)
    return put_code(0xA1 + (cp - 0xFF61), 1, out, outleft, made);
  if (cp >= 0xE000 && cp < 0xE000 + 10 * 188) {
    const unsigned index = cp - 0xE000;
    const unsigned column = index % 188;
    const unsigned trail = 0x40 + column + (column >= 0x3F ? 1 : 0);
    return put_code((0xF0 + index / 188) << 8 | trail, 2, out, outleft, made);
  }
  for (size_t i = 0; i < CP932_CHANGE_COUNT; ++i)
    if (CP932_CHANGES[i].code_point == cp)
      return put_code(CP932_CHANGES[i].code, 2, out, outleft, made);
  // JIS X 0208 also finds the characters CP932 changed, which glibc still
  // writes as their JIS X 0208 codes.
  if (uint16_t code = find_code(JIS_X0208, cp))
    return put_code(shift_jis_from_jis(code), 2, out, outleft, made);
  const CodeTable *const extensions[] = {&CP932_NEC_ROW_13, &CP932_IBM,
                                         &CP932_NEC_SELECTED_IBM};
  for (const CodeTable *table : extensions)
    if (uint16_t code = find_code(*table, cp))
      return put_code(code, 2, out, outleft, made);
  // As does JIS X 0201's yen sign and overline, and an em dash as the
  // horizontal bar JIS X 0208 has in its place.
  if (cp == 0xA5)
    return put_code(0x5C, 1, out, outleft, made);
  if (cp == 0x203E)
    return put_code(0x7E, 1, out, outleft, made);
  if (cp == 0x2014)
    return put_code(0x815C, 2, out, outleft, made);
  return Status::INVALID;
}

} // namespace iconv_internal
} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC_ICONV_JAPANESE_H
