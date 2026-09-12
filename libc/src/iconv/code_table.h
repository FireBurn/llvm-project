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
