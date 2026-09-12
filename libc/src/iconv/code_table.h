//===-- Tables of two byte codes for iconv's sets ---------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// A table of the characters at a block of two byte codes: a range of lead
/// bytes, each followed by a range of trail bytes. Reading looks a code up by
/// position; writing searches the positions, which are kept in code point
/// order.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC_ICONV_CODE_TABLE_H
#define LLVM_LIBC_SRC_ICONV_CODE_TABLE_H

#include "hdr/stdint_proxy.h"
#include "hdr/types/char32_t.h"
#include "hdr/types/size_t.h"
#include "src/__support/macros/attributes.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {
namespace iconv_internal {

// A code and the character it is, for codes a table cannot hold.
struct CodePair {
  uint16_t code;
  uint16_t code_point;
};

// The codes from |first| to |last|.
struct CodeRange {
  uint16_t first;
  uint16_t last;
};

// Where a run of consecutive code points begins among a set's codes.
struct CodeRun {
  uint16_t position;
  uint16_t code_point;
};

struct CodeTable {
  uint8_t lead_first;
  uint8_t lead_count;
  uint8_t trail_first;
  uint8_t trail_count;
  // The code point at each code, row by row, or 0 where there is none.
  const uint16_t *code_points;
  // The positions of the characters which are written as their code here, in
  // order of code point. A character the set writes as another code is only
  // read.
  const uint16_t *order;
  uint16_t count;
};

// Codes and the values they stand for, in order of code, where too few codes
// have characters for a table of rows and columns.
struct CodeList {
  const CodePair *pairs;
  // The positions of the pairs, in order of value.
  const uint16_t *order;
  uint16_t count;
};

// Whether |list| has |code|, and the value it stands for.
LIBC_INLINE bool look_up(const CodeList &list, unsigned code, uint16_t &value) {
  size_t low = 0;
  size_t high = list.count;
  while (low < high) {
    size_t mid = low + (high - low) / 2;
    if (list.pairs[mid].code < code)
      low = mid + 1;
    else
      high = mid;
  }
  if (low == list.count || list.pairs[low].code != code)
    return false;
  value = list.pairs[low].code_point;
  return true;
}

// The code which stands for |value| in |list|, or 0.
LIBC_INLINE uint16_t find_code(const CodeList &list, unsigned value) {
  size_t low = 0;
  size_t high = list.count;
  while (low < high) {
    size_t mid = low + (high - low) / 2;
    if (list.pairs[list.order[mid]].code_point < value)
      low = mid + 1;
    else
      high = mid;
  }
  if (low == list.count)
    return 0;
  const CodePair &pair = list.pairs[list.order[low]];
  return pair.code_point == value ? pair.code : 0;
}

// A set of 94 rows of 94 characters from 0x2121, some of them beyond the Basic
// Multilingual Plane.
struct WideTable {
  // The low 16 bits of the code point at each code, row by row.
  const uint16_t *code_points;
  // The plane of each of those, two bits to a code and four codes to a byte.
  const uint8_t *planes;
  // The positions of the characters, in order of code point.
  const uint16_t *order;
  uint16_t count;
};

// The character at |position| in |table|, or 0.
LIBC_INLINE char32_t wide_character(const WideTable &table, unsigned position) {
  const unsigned plane = (table.planes[position / 4] >> (position % 4 * 2)) & 3;
  return static_cast<char32_t>(plane << 16 | table.code_points[position]);
}

// The character at |row| and |column|, or 0.
LIBC_INLINE char32_t look_up(const WideTable &table, unsigned row,
                             unsigned column) {
  if (row - 0x21 >= 94 || column - 0x21 >= 94)
    return 0;
  return wide_character(table, (row - 0x21) * 94 + (column - 0x21));
}

// The code |cp| has in |table|, or 0.
LIBC_INLINE uint16_t find_code(const WideTable &table, char32_t cp) {
  if (cp == 0)
    return 0;
  size_t low = 0;
  size_t high = table.count;
  while (low < high) {
    size_t mid = low + (high - low) / 2;
    if (wide_character(table, table.order[mid]) < cp)
      low = mid + 1;
    else
      high = mid;
  }
  if (low == table.count || wide_character(table, table.order[low]) != cp)
    return 0;
  const unsigned position = table.order[low];
  return static_cast<uint16_t>((0x21 + position / 94) << 8 |
                               (0x21 + position % 94));
}

// The character at |lead| and |trail|, or 0.
LIBC_INLINE char32_t look_up(const CodeTable &table, unsigned lead,
                             unsigned trail) {
  unsigned row = lead - table.lead_first;
  unsigned column = trail - table.trail_first;
  if (row >= table.lead_count || column >= table.trail_count)
    return 0;
  return table.code_points[row * table.trail_count + column];
}

// The code |cp| is written as, or 0 if the table does not write it.
LIBC_INLINE uint16_t find_code(const CodeTable &table, char32_t cp) {
  if (cp == 0 || cp > 0xFFFF)
    return 0;
  size_t low = 0;
  size_t high = table.count;
  while (low < high) {
    size_t mid = low + (high - low) / 2;
    if (table.code_points[table.order[mid]] < cp)
      low = mid + 1;
    else
      high = mid;
  }
  if (low == table.count || table.code_points[table.order[low]] != cp)
    return 0;
  const unsigned position = table.order[low];
  return static_cast<uint16_t>(
      (table.lead_first + position / table.trail_count) << 8 |
      (table.trail_first + position % table.trail_count));
}

} // namespace iconv_internal
} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC_ICONV_CODE_TABLE_H
