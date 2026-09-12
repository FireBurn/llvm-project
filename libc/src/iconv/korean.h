//===-- The Korean sets of iconv --------------------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// EUC-KR, CP949 and JOHAB, which share KS X 1001's table. CP949 adds the
/// Hangul syllables KS X 1001 lacks, in order after it, and JOHAB builds each
/// syllable from its letters. |used| is how many bytes a character took, or
/// for input which is not a character, how many are skipped, which follows
/// glibc.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC_ICONV_KOREAN_H
#define LLVM_LIBC_SRC_ICONV_KOREAN_H

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

// EUC-KR: ASCII, and KS X 1001 with both bytes from 0xA1. As in glibc, the
// other C1 bytes are the C1 controls.
LIBC_INLINE Status read_euc_kr(const unsigned char *in, size_t inleft,
                               char32_t &out, size_t &used) {
  const unsigned lead = in[0];
  used = 1;
  if (lead < 0xA0) {
    out = lead;
    return Status::OK;
  }
  if (lead == 0xA0)
    return Status::INVALID;
  if (inleft < 2)
    return Status::INCOMPLETE;
  used = 2;
  char32_t cp = look_up(KS_X_1001, lead - 0x80, in[1] - 0x80);
  if (cp == 0)
    return Status::INVALID;
  out = cp;
  return Status::OK;
}

LIBC_INLINE Status write_euc_kr(char32_t cp, unsigned char *out, size_t outleft,
                                size_t &made) {
  if (cp < 0xA0)
    return put_code(cp, 1, out, outleft, made);
  // glibc writes the won sign as the fullwidth one KS X 1001 has.
  if (cp == 0x20A9)
    cp = 0xFFE6;
  if (uint16_t code = find_code(KS_X_1001, cp))
    return put_code(code | 0x8080, 2, out, outleft, made);
  return Status::INVALID;
}

// KS X 1001's Hangul syllables fill rows 16 to 40, in order.
constexpr unsigned KS_X_1001_HANGUL_COUNT = 25 * 94;

LIBC_INLINE char32_t ks_x_1001_hangul(unsigned index) {
  return look_up(KS_X_1001, 0x30 + index / 94, 0x21 + index % 94);
}

// How many of KS X 1001's syllables come before |cp|.
LIBC_INLINE unsigned hangul_before(char32_t cp) {
  unsigned low = 0;
  unsigned high = KS_X_1001_HANGUL_COUNT;
  while (low < high) {
    unsigned mid = low + (high - low) / 2;
    if (ks_x_1001_hangul(mid) < cp)
      low = mid + 1;
    else
      high = mid;
  }
  return low;
}

// The |n|th Hangul syllable KS X 1001 lacks, counting from 0. Before the
// syllable at |index| in KS X 1001, it lacks that syllable's offset less
// |index|.
LIBC_INLINE char32_t missing_hangul(unsigned n) {
  unsigned low = 0;
  unsigned high = KS_X_1001_HANGUL_COUNT;
  while (low < high) {
    unsigned mid = low + (high - low) / 2;
    if (ks_x_1001_hangul(mid) - 0xAC00 - mid <= n)
      low = mid + 1;
    else
      high = mid;
  }
  return 0xAC00 + n + low;
}

// CP949: EUC-KR's KS X 1001 without the postal code mark, and the syllables
// it lacks at the codes before and beside it, whose trail bytes are letters
// or from 0x81.
constexpr unsigned CP949_HANGUL_COUNT = 11172 - KS_X_1001_HANGUL_COUNT;
constexpr unsigned CP949_LONG_ROWS = 0xA1 - 0x81;

LIBC_INLINE Status read_cp949(const unsigned char *in, size_t inleft,
                              char32_t &out, size_t &used) {
  const unsigned lead = in[0];
  used = 1;
  if (lead < 0x80) {
    out = lead;
    return Status::OK;
  }
  // There is no code after 0x80 or 0xFF, and those after 0xC9 and 0xFE are
  // user-defined.
  if (lead == 0x80 || lead == 0xC9 || lead >= 0xFE)
    return Status::INVALID;
  if (inleft < 2)
    return Status::INCOMPLETE;
  const unsigned trail = in[1];
  char32_t cp = 0;
  if (lead >= 0xA1 && trail >= 0xA1) {
    const unsigned code = (lead << 8 | trail) & 0x7F7F;
    if (code != 0x2268)
      cp = look_up(KS_X_1001, code >> 8, code & 0xFF);
  } else {
    unsigned column = 256;
    if (trail >= 0x41 && trail <= 0x5A)
      column = trail - 0x41;
    else if (trail >= 0x61 && trail <= 0x7A)
      column = 26 + (trail - 0x61);
    else if (trail >= 0x81 && trail <= 0xFE)
      column = 52 + (trail - 0x81);
    unsigned n = CP949_HANGUL_COUNT;
    if (lead < 0xA1 && column < 178)
      n = (lead - 0x81) * 178 + column;
    else if (lead >= 0xA1 && column < 84)
      n = CP949_LONG_ROWS * 178 + (lead - 0xA1) * 84 + column;
    if (n < CP949_HANGUL_COUNT)
      cp = missing_hangul(n);
  }
  if (cp == 0) {
    // As in glibc, a code KS X 1001 would have is skipped whole, and any other
    // by its lead byte alone.
    if (trail >= 0xA1 && trail <= 0xFE)
      used = 2;
    return Status::INVALID;
  }
  out = cp;
  used = 2;
  return Status::OK;
}

LIBC_INLINE Status write_cp949(char32_t cp, unsigned char *out, size_t outleft,
                               size_t &made) {
  if (cp < 0x80)
    return put_code(cp, 1, out, outleft, made);
  if (cp >= 0xAC00 && cp <= 0xD7A3) {
    const unsigned before = hangul_before(cp);
    if (ks_x_1001_hangul(before) != cp) {
      const unsigned n = (cp - 0xAC00) - before;
      unsigned lead;
      unsigned column;
      if (n < CP949_LONG_ROWS * 178) {
        lead = 0x81 + n / 178;
        column = n % 178;
      } else {
        lead = 0xA1 + (n - CP949_LONG_ROWS * 178) / 84;
        column = (n - CP949_LONG_ROWS * 178) % 84;
      }
      const unsigned trail = column < 26   ? 0x41 + column
                             : column < 52 ? 0x61 + (column - 26)
                                           : 0x81 + (column - 52);
      return put_code(lead << 8 | trail, 2, out, outleft, made);
    }
  }
  uint16_t code = find_code(KS_X_1001, cp);
  if (code == 0 || code == 0x2268)
    return Status::INVALID;
  return put_code(code | 0x8080, 2, out, outleft, made);
}

// JOHAB: a Hangul syllable is 1, then five bits each for its first, middle
// and last letter, where a value of 1 or 2 is no letter. KS X 1001's symbols
// and hanja are moved to leads from 0xD9 and 0xE0, two of its rows to each.
constexpr uint8_t JOHAB_NONE = 0xFF;
constexpr uint8_t JOHAB_MEDIAL_INDEX[32] = {
    JOHAB_NONE, JOHAB_NONE, JOHAB_NONE, 0,  1,  2,  3,          4,
    JOHAB_NONE, JOHAB_NONE, 5,          6,  7,  8,  9,          10,
    JOHAB_NONE, JOHAB_NONE, 11,         12, 13, 14, 15,         16,
    JOHAB_NONE, JOHAB_NONE, 17,         18, 19, 20, JOHAB_NONE, JOHAB_NONE};
constexpr uint8_t JOHAB_MEDIAL_VALUE[21] = {3,  4,  5,  6,  7,  10, 11,
                                            12, 13, 14, 15, 18, 19, 20,
                                            21, 22, 23, 26, 27, 28, 29};
constexpr uint8_t JOHAB_FINAL_VALUE[28] = {
    1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13, 14,
    15, 16, 17, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29};

LIBC_INLINE char32_t johab_hangul(unsigned code) {
  const unsigned initial = (code >> 10) & 31;
  const unsigned medial = JOHAB_MEDIAL_INDEX[(code >> 5) & 31];
  const unsigned final = code & 31;
  if (initial >= 2 && initial <= 20 && medial != JOHAB_NONE && final >= 1 &&
      final <= 29 && final != 18) {
    const unsigned last = final < 18 ? final - 1 : final - 2;
    return 0xAC00 + ((initial - 2) * 21 + medial) * 28 + last;
  }
  for (size_t i = 0; i < JOHAB_LETTER_COUNT; ++i)
    if (JOHAB_LETTERS[i].code == code)
      return JOHAB_LETTERS[i].code_point;
  return 0;
}

// The KS X 1001 letters JOHAB has only as codes of one letter.
constexpr unsigned JOHAB_LETTER_COLUMNS = 51;

LIBC_INLINE char32_t johab_symbol(unsigned lead, unsigned trail) {
  unsigned row = lead <= 0xDE ? 2 * (lead - 0xD9) + 1 : 42 + 2 * (lead - 0xE0);
  unsigned column;
  if (trail >= 0x31 && trail <= 0x7E) {
    column = trail - 0x30;
  } else if (trail >= 0x91 && trail <= 0xA0) {
    column = trail - 0x42;
  } else if (trail >= 0xA1 && trail <= 0xFE) {
    ++row;
    column = trail - 0xA0;
  } else {
    return 0;
  }
  if (row == 4 && column <= JOHAB_LETTER_COLUMNS)
    return 0;
  return look_up(KS_X_1001, 0x20 + row, 0x20 + column);
}

LIBC_INLINE Status read_johab(const unsigned char *in, size_t inleft,
                              char32_t &out, size_t &used) {
  const unsigned lead = in[0];
  used = 1;
  if (lead < 0x80) {
    out = lead == 0x5C ? 0x20A9 : lead;
    return Status::OK;
  }
  const bool hangul = lead >= 0x84 && lead <= 0xD3;
  const bool symbol =
      (lead >= 0xD9 && lead <= 0xDE) || (lead >= 0xE0 && lead <= 0xF9);
  if (!hangul && !symbol)
    return Status::INVALID;
  if (inleft < 2)
    return Status::INCOMPLETE;
  const unsigned code = lead << 8 | in[1];
  char32_t cp = hangul ? johab_hangul(code) : johab_symbol(lead, in[1]);
  if (cp == 0) {
    // glibc skips a code with no first or middle letter but a last one as a
    // whole, and others by their lead byte alone.
    const unsigned final = code & 31;
    if (hangul && (code >> 5 & 0x3FF) == 0x22 && final >= 2 && final <= 29 &&
        final != 18)
      used = 2;
    // A symbol code with a trail byte JOHAB uses is skipped whole too, but not
    // a letter KS X 1001 has there, nor one past the last code of the first or
    // last lead byte.
    const unsigned trail = in[1];
    const bool trail_used =
        (trail >= 0x31 && trail <= 0x7E) || (trail >= 0x91 && trail <= 0xFE);
    const bool letter =
        lead == 0xDA && trail >= 0xA1 && trail < 0xA1 + JOHAB_LETTER_COLUMNS;
    if (symbol && trail_used && !letter && !(lead == 0xD9 && trail > 0xE8) &&
        !(lead == 0xDE && trail > 0xF1))
      used = 2;
    return Status::INVALID;
  }
  out = cp;
  used = 2;
  return Status::OK;
}

LIBC_INLINE Status write_johab(char32_t cp, unsigned char *out, size_t outleft,
                               size_t &made) {
  // The won sign is JOHAB's 0x5C, and ASCII's reverse solidus has no place.
  if (cp == 0x20A9)
    return put_code(0x5C, 1, out, outleft, made);
  if (cp < 0x80 && cp != 0x5C)
    return put_code(cp, 1, out, outleft, made);
  if (cp >= 0xAC00 && cp <= 0xD7A3) {
    const unsigned s = cp - 0xAC00;
    const unsigned code = 0x8000 | (2 + s / 588) << 10 |
                          JOHAB_MEDIAL_VALUE[(s / 28) % 21] << 5 |
                          JOHAB_FINAL_VALUE[s % 28];
    return put_code(code, 2, out, outleft, made);
  }
  for (size_t i = 0; i < JOHAB_LETTER_COUNT; ++i)
    if (JOHAB_LETTERS[i].code_point == cp)
      return put_code(JOHAB_LETTERS[i].code, 2, out, outleft, made);
  const uint16_t code = find_code(KS_X_1001, cp);
  const unsigned row = (code >> 8) - 0x20;
  const unsigned column = (code & 0xFF) - 0x20;
  if (code == 0 || (row > 12 && row < 42) || row > 93)
    return Status::INVALID;
  unsigned lead;
  unsigned trail;
  if (row <= 12) {
    lead = 0xD8 + (row + 1) / 2;
  } else {
    lead = 0xE0 + (row - 42) / 2;
  }
  const bool first = row <= 12 ? row % 2 == 1 : row % 2 == 0;
  if (!first)
    trail = column + 0xA0;
  else
    trail = column + (column <= 0x4E ? 0x30 : 0x42);
  return put_code(lead << 8 | trail, 2, out, outleft, made);
}

} // namespace iconv_internal
} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC_ICONV_KOREAN_H
