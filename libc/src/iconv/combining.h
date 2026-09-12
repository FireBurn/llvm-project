//===-- Letters and combining marks in iconv's sets -------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Some sets write an accented letter as the letter followed by a combining
/// mark. Reading one joins the two back into a single character where Unicode
/// has one, and writing a character the set has no byte for splits it.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC_ICONV_COMBINING_H
#define LLVM_LIBC_SRC_ICONV_COMBINING_H

#include "hdr/stdint_proxy.h"
#include "hdr/types/char32_t.h"
#include "hdr/types/size_t.h"
#include "src/__support/macros/attributes.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {
namespace iconv_internal {

// A character, a combining mark after it, and the character the two make.
struct Composition {
  uint16_t base;
  uint16_t mark;
  uint16_t composed;
};

// A character a set has no byte for, and the bytes of the letter and marks it
// is written as.
struct Decomposition {
  uint16_t composed;
  uint8_t length;
  uint8_t bytes[3];
};

// The pairs a set joins, in order of base and then mark, and how it writes
// the characters it has no byte for, in order of character.
struct Combining {
  const Composition *compositions;
  size_t composition_count;
  const Decomposition *decompositions;
  size_t decomposition_count;
};

// The first pair which is not before |base| and |mark|, or the end.
LIBC_INLINE size_t find_pair(const Combining &combining, char32_t base,
                             char32_t mark) {
  size_t low = 0;
  size_t high = combining.composition_count;
  while (low < high) {
    size_t mid = low + (high - low) / 2;
    const Composition &pair = combining.compositions[mid];
    if (pair.base < base || (pair.base == base && pair.mark < mark))
      low = mid + 1;
    else
      high = mid;
  }
  return low;
}

// Whether some mark joins |cp|, so that reading it has to wait for the next
// character.
LIBC_INLINE bool takes_marks(const Combining &combining, char32_t cp) {
  size_t i = find_pair(combining, cp, 0);
  return i < combining.composition_count &&
         combining.compositions[i].base == cp;
}

// The character |base| and |mark| make together, or 0 if they stay apart.
LIBC_INLINE char32_t compose(const Combining &combining, char32_t base,
                             char32_t mark) {
  size_t i = find_pair(combining, base, mark);
  if (i == combining.composition_count)
    return 0;
  const Composition &pair = combining.compositions[i];
  return pair.base == base && pair.mark == mark ? pair.composed : 0;
}

// How |cp| is written as a letter and marks, or null if it cannot be.
LIBC_INLINE const Decomposition *decompose(const Combining &combining,
                                           char32_t cp) {
  size_t low = 0;
  size_t high = combining.decomposition_count;
  while (low < high) {
    size_t mid = low + (high - low) / 2;
    if (combining.decompositions[mid].composed < cp)
      low = mid + 1;
    else
      high = mid;
  }
  if (low == combining.decomposition_count ||
      combining.decompositions[low].composed != cp)
    return nullptr;
  return &combining.decompositions[low];
}

} // namespace iconv_internal
} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC_ICONV_COMBINING_H
