//===-- Implementation of strverscmp --------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/string/strverscmp.h"

#include "hdr/stdint_proxy.h"
#include "src/__support/common.h"
#include "src/__support/ctype_utils.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {

namespace {

// Comparing two strings which hold a version number is not the same as
// comparing them as text: "file10" comes after "file9" even though '1' is
// less than '9'. A run of digits stands for the number it spells, except
// that a run which starts with a zero stands for a fraction, and a fraction
// sorts before a whole number.
//
// Which of those a run is depends on what came before it, so the comparison
// walks the two strings in one of four states, and the state it is in when
// they first differ decides what the difference means.

// The states: neither string is in a number, both are in a whole number,
// both are in a fraction, and both are in the leading zeros of one.
constexpr int S_N = 0x0;
constexpr int S_I = 0x3;
constexpr int S_F = 0x6;
constexpr int S_Z = 0x9;

// What the difference means: compare the characters, or compare how long
// the two runs of digits are.
constexpr int CMP = 2;
constexpr int LEN = 3;

// The state to move to, given the character just read: something other than
// a digit, a digit which is not zero, or a zero.
constexpr uint8_t NEXT_STATE[] = {
    /* state    x    d    0  */
    /* S_N */ S_N, S_I, S_Z,
    /* S_I */ S_N, S_I, S_I,
    /* S_F */ S_N, S_F, S_F,
    /* S_Z */ S_N, S_F, S_Z,
};

// What to make of the first difference, given the state and what each of
// the two differing characters is.
constexpr int8_t RESULT_TYPE[] = {
    /* state   x/x  x/d  x/0  d/x  d/d  d/0  0/x  0/d  0/0  */
    /* S_N */ CMP, CMP, CMP, CMP, LEN, CMP, CMP, CMP, CMP,
    /* S_I */ CMP, -1,  -1,  +1,  LEN, LEN, +1,  LEN, LEN,
    /* S_F */ CMP, CMP, CMP, CMP, CMP, CMP, CMP, CMP, CMP,
    /* S_Z */ CMP, +1,  +1,  -1,  CMP, CMP, -1,  CMP, CMP,
};

// How a character moves the state along: 0 for anything else, 1 for a digit
// which is not zero, 2 for a zero.
LIBC_INLINE int step(unsigned char c) {
  return (c == '0') + (internal::isdigit(static_cast<char>(c)) ? 1 : 0);
}

} // anonymous namespace

LLVM_LIBC_FUNCTION(int, strverscmp, (const char *lhs, const char *rhs)) {
  const unsigned char *left = reinterpret_cast<const unsigned char *>(lhs);
  const unsigned char *right = reinterpret_cast<const unsigned char *>(rhs);
  if (left == right)
    return 0;

  unsigned char lc = *left++;
  unsigned char rc = *right++;
  int state = S_N + step(lc);

  int diff;
  while ((diff = lc - rc) == 0) {
    if (lc == '\0')
      return diff;
    state = NEXT_STATE[state];
    lc = *left++;
    rc = *right++;
    state += step(lc);
  }

  state = RESULT_TYPE[state * 3 + step(rc)];

  switch (state) {
  case CMP:
    return diff;
  case LEN:
    // Both are in a whole number, so the one with more digits left is the
    // larger, and only if they run out together does the first differing
    // digit decide it.
    while (internal::isdigit(static_cast<char>(*left++)))
      if (!internal::isdigit(static_cast<char>(*right++)))
        return 1;
    return internal::isdigit(static_cast<char>(*right)) ? -1 : diff;
  default:
    return state;
  }
}

} // namespace LIBC_NAMESPACE_DECL
