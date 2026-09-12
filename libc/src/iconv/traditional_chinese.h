//===-- The Traditional Chinese sets of iconv -------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// BIG5, which glibc also calls CP950, BIG5-HKSCS and EUC-TW. |used| is how
/// many bytes a character took, or for input which is not a character, how many
/// are skipped, which follows glibc.
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
#include "src/iconv/sequences.h"
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

// BIG5-HKSCS: Big5 with the Hong Kong Supplementary Character Set, whose codes
// have lead bytes from 0x87, and 0x80 by itself. glibc has only the 2008
// edition. The earlier ones GNU libiconv has are the same without the
// characters later editions added, and are read and written as glibc reads
// and writes the 2008 edition.
struct HkscsEdition {
  uint16_t first;
  uint16_t last;
  uint16_t year;
};

// The codes each edition after 1999 added, newest first.
constexpr HkscsEdition HKSCS_EDITIONS[] = {
    {0x877A, 0x87DF, 2008}, {0x8740, 0x8779, 2004}, {0x8C62, 0x8C62, 2004},
    {0x8CDB, 0x8CDB, 2004}, {0x8CDD, 0x8D5F, 2004}, {0x8C40, 0x8CDC, 2001},
};

// The year of the edition which added |code|, or 1999 for the rest.
LIBC_INLINE unsigned hkscs_edition(unsigned code) {
  for (const HkscsEdition &edition : HKSCS_EDITIONS)
    if (code >= edition.first && code <= edition.last)
      return edition.year;
  return 1999;
}

// The characters of the Supplementary Ideographic Plane are kept apart, as
// their offset from U+20000.
LIBC_INLINE char32_t hkscs_plane_2_character(unsigned code) {
  uint16_t offset = 0;
  if (!look_up(BIG5_HKSCS_PLANE_2, code, offset))
    return 0;
  return 0x20000 + offset;
}

LIBC_INLINE uint16_t hkscs_plane_2_code(char32_t cp) {
  if (cp < 0x20000 || cp > 0x2FFFF)
    return 0;
  return find_code(BIG5_HKSCS_PLANE_2, cp - 0x20000);
}

LIBC_INLINE Status read_big5_hkscs(unsigned year, const unsigned char *in,
                                   size_t inleft, char32_t &out, size_t &used) {
  const unsigned lead = in[0];
  used = 1;
  if (lead <= 0x80) {
    out = lead;
    return Status::OK;
  }
  if (lead == 0xFF)
    return Status::INVALID;
  if (inleft < 2)
    return Status::INCOMPLETE;
  // As in glibc, a pair with no character is skipped by its lead byte.
  const unsigned trail = in[1];
  const unsigned code = lead << 8 | trail;
  if (hkscs_edition(code) > year)
    return Status::INVALID;
  if (find_code(BIG5_HKSCS_SEQUENCES, static_cast<uint16_t>(code))) {
    used = 2;
    out = code;
    return Status::SEQUENCE;
  }
  char32_t cp = look_up(BIG5_HKSCS, lead, trail);
  if (cp == 0)
    cp = hkscs_plane_2_character(code);
  if (cp == 0)
    return Status::INVALID;
  used = 2;
  out = cp;
  return Status::OK;
}

LIBC_INLINE Status write_big5_hkscs(unsigned year, char32_t cp,
                                    unsigned char *out, size_t outleft,
                                    size_t &made) {
  if (cp <= 0x80)
    return put_code(cp, 1, out, outleft, made);
  const uint16_t code =
      cp > 0xFFFF ? hkscs_plane_2_code(cp) : find_code(BIG5_HKSCS, cp);
  if (code == 0 || hkscs_edition(code) > year)
    return Status::INVALID;
  return put_code(code, 2, out, outleft, made);
}

// EUC-TW: ASCII, plane 1 of CNS 11643 with both bytes from 0xA1, and a plane
// from 1 to 16 as 0x8E, 0xA0 plus the plane, and two bytes from 0xA1. glibc
// has planes 1 to 7 and 15.
LIBC_INLINE const WideTable *cns_plane(unsigned plane) {
  if (plane >= 1 && plane <= 7)
    return &CNS_11643[plane - 1];
  return plane == 15 ? &CNS_11643[7] : nullptr;
}

LIBC_INLINE Status read_euc_tw(const unsigned char *in, size_t inleft,
                               char32_t &out, size_t &used) {
  const unsigned lead = in[0];
  used = 1;
  if (lead < 0x80) {
    out = lead;
    return Status::OK;
  }
  if (lead == 0x8E) {
    if (inleft < 2)
      return Status::INCOMPLETE;
    const unsigned plane = in[1];
    if (plane < 0xA1 || plane > 0xB0)
      return Status::INVALID;
    if (inleft < 4)
      return Status::INCOMPLETE;
    // As in glibc, a four byte code with no character is skipped by its first
    // byte.
    const WideTable *table = cns_plane(plane - 0xA0);
    if (table == nullptr || in[2] < 0xA1 || in[3] < 0xA1)
      return Status::INVALID;
    const char32_t cp = look_up(*table, in[2] - 0x80, in[3] - 0x80);
    if (cp == 0)
      return Status::INVALID;
    used = 4;
    out = cp;
    return Status::OK;
  }
  if (lead < 0xA1 || lead == 0xFF)
    return Status::INVALID;
  if (inleft < 2)
    return Status::INCOMPLETE;
  const unsigned trail = in[1];
  if (trail < 0xA1 || trail == 0xFF)
    return Status::INVALID;
  // A pair with no character is skipped whole.
  used = 2;
  const char32_t cp = look_up(CNS_11643[0], lead - 0x80, trail - 0x80);
  if (cp == 0)
    return Status::INVALID;
  out = cp;
  return Status::OK;
}

LIBC_INLINE Status write_euc_tw(char32_t cp, unsigned char *out, size_t outleft,
                                size_t &made) {
  if (cp < 0x80)
    return put_code(cp, 1, out, outleft, made);
  for (unsigned plane = 1; plane <= 15; ++plane) {
    const WideTable *table = cns_plane(plane);
    if (table == nullptr)
      continue;
    const uint16_t code = find_code(*table, cp);
    if (code == 0)
      continue;
    if (plane == 1)
      return put_code(code | 0x8080, 2, out, outleft, made);
    return put_code(0x8E000000 | (0xA0 + plane) << 16 | code | 0x8080, 4, out,
                    outleft, made);
  }
  return Status::INVALID;
}

} // namespace iconv_internal
} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC_ICONV_TRADITIONAL_CHINESE_H
