//===-- Codes which stand for several characters in iconv -------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Some sets have codes which stand for a sequence of Unicode characters, so
/// reading one gives all of them, and writing waits for the characters after
/// one which may begin a sequence.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC_ICONV_SEQUENCES_H
#define LLVM_LIBC_SRC_ICONV_SEQUENCES_H

#include "hdr/stdint_proxy.h"
#include "hdr/types/char32_t.h"
#include "hdr/types/size_t.h"
#include "src/__support/macros/attributes.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {
namespace iconv_internal {

// A code and the characters it stands for.
struct Sequence {
  uint16_t code;
  uint8_t length;
  uint16_t code_points[3];
};

struct Sequences {
  const Sequence *entries;
  size_t count;
};

// The sequence |code| stands for.
LIBC_INLINE const Sequence *find_code(const Sequences &sequences,
                                      uint16_t code) {
  for (size_t i = 0; i < sequences.count; ++i)
    if (sequences.entries[i].code == code)
      return &sequences.entries[i];
  return nullptr;
}

// How characters written so far stand against a set's sequences.
enum class Match {
  NONE,   // They are not the start of any sequence.
  PREFIX, // They begin a longer sequence.
  WHOLE,  // They are a sequence, and begin no longer one.
};

// Compares |code_points| with each sequence. |whole| is the sequence they are,
// if they are one, whether or not they also begin a longer one.
LIBC_INLINE Match match(const Sequences &sequences, const char32_t *code_points,
                        size_t length, const Sequence *&whole) {
  Match result = Match::NONE;
  whole = nullptr;
  for (size_t i = 0; i < sequences.count; ++i) {
    const Sequence &sequence = sequences.entries[i];
    if (sequence.length < length)
      continue;
    size_t same = 0;
    while (same < length && sequence.code_points[same] == code_points[same])
      ++same;
    if (same < length)
      continue;
    if (sequence.length > length) {
      result = Match::PREFIX;
      continue;
    }
    whole = &sequence;
    if (result == Match::NONE)
      result = Match::WHOLE;
  }
  return result;
}

} // namespace iconv_internal
} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC_ICONV_SEQUENCES_H
