//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// The generator behind random, srandom, initstate and setstate.
///
/// This is the additive feedback generator the BSDs introduced: each value
/// is the sum of two earlier ones taken from a table, and the size of the
/// table decides how long the sequence runs before it repeats. The smallest
/// table has no room for that and is a linear congruential generator
/// instead.
///
/// The table sizes, the trinomials and the way a seed is spread over the
/// table are the ones every other implementation uses, so a program seeded
/// the same way gets the same numbers here as it does elsewhere.
///
/// A caller's own table, from initstate, holds one word in front of the
/// table itself which records which kind of table it is and how far through
/// the sequence it had got, so setstate can take it up again.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC_STDLIB_RANDOM_UTIL_H
#define LLVM_LIBC_SRC_STDLIB_RANDOM_UTIL_H

#include "hdr/stdint_proxy.h"
#include "hdr/types/size_t.h"
#include "src/__support/macros/attributes.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {
namespace random_internal {

// How many earlier values each kind of table adds together, and how far
// apart the two it uses are.
struct Kind {
  int degree;
  int separation;
};

constexpr int KIND_COUNT = 5;
constexpr Kind KINDS[KIND_COUNT] = {
    {0, 0}, {7, 3}, {15, 1}, {31, 3}, {63, 1},
};

// The table sizes which select each kind, in bytes, counting the word in
// front which says what the table is.
constexpr size_t KIND_BYTES[KIND_COUNT] = {8, 32, 64, 128, 256};

// The kind a table of |size| bytes holds: the largest which fits.
LIBC_INLINE int kind_for_size(size_t size) {
  int kind = 0;
  for (int i = KIND_COUNT - 1; i >= 0; --i) {
    if (size >= KIND_BYTES[i]) {
      kind = i;
      break;
    }
  }
  return kind;
}

struct State {
  int32_t *table; // The table itself, one word past what a caller holds.
  int kind;
  int degree;
  int separation;
  int front; // Where the next value is accumulated.
  int rear;  // The one |separation| behind it which is added in.
};

extern int32_t default_table[KINDS[3].degree + 1];
extern State state;
extern bool seeded;

LIBC_INLINE void configure(State &s, int kind) {
  s.kind = kind;
  s.degree = KINDS[kind].degree;
  s.separation = KINDS[kind].separation;
}

// Produces the next value. The low bit is dropped because the addition only
// randomises the bits above it.
LIBC_INLINE int32_t next(State &s) {
  if (s.kind == 0) {
    uint32_t value =
        (static_cast<uint32_t>(s.table[0]) * 1103515245u + 12345u) &
        0x7FFFFFFFu;
    s.table[0] = static_cast<int32_t>(value);
    return static_cast<int32_t>(value);
  }

  // The addition is meant to wrap, which is what makes the sequence work, so
  // it happens in the unsigned type where wrapping is defined.
  uint32_t value = static_cast<uint32_t>(s.table[s.front]) +
                   static_cast<uint32_t>(s.table[s.rear]);
  s.table[s.front] = static_cast<int32_t>(value);

  if (++s.front >= s.degree)
    s.front = 0;
  if (++s.rear >= s.degree)
    s.rear = 0;
  return static_cast<int32_t>(value >> 1);
}

// Fills the table from a seed and runs the generator far enough that the
// seed is no longer visible in what comes out.
LIBC_INLINE void seed(State &s, unsigned int value) {
  // A seed of zero would leave the multiplicative generator stuck there.
  if (value == 0)
    value = 1;
  s.table[0] = static_cast<int32_t>(value);
  if (s.kind == 0)
    return;

  // Each entry comes from the multiplicative generator Lehmer described,
  // evaluated the way Schrage showed so that it never overflows.
  int32_t word = static_cast<int32_t>(value);
  for (int i = 1; i < s.degree; ++i) {
    int32_t high = word / 127773;
    int32_t low = word % 127773;
    word = 16807 * low - 2836 * high;
    if (word < 0)
      word += 2147483647;
    s.table[i] = word;
  }

  s.front = s.separation;
  s.rear = 0;
  for (int i = s.degree * 10; i > 0; --i)
    next(s);
}

// Records in the word in front of the table which kind it is and how far
// through the sequence it had got, so setstate can take it up again.
LIBC_INLINE void save(const State &s) {
  if (s.kind == 0)
    s.table[-1] = 0;
  else
    s.table[-1] = static_cast<int32_t>(KIND_COUNT * s.rear + s.kind);
}

// Reads that word back and takes the table up where it was left.
LIBC_INLINE bool adopt(State &s, int32_t *caller_state) {
  int32_t info = caller_state[0];
  int kind = static_cast<int>(info % KIND_COUNT);
  if (kind < 0 || kind >= KIND_COUNT)
    return false;
  configure(s, kind);
  s.table = caller_state + 1;
  if (kind == 0) {
    s.front = 0;
    s.rear = 0;
    return true;
  }
  int rear = static_cast<int>(info / KIND_COUNT);
  if (rear < 0 || rear >= s.degree)
    return false;
  s.rear = rear;
  s.front = (rear + s.separation) % s.degree;
  return true;
}

// A program which never seeds behaves as though it had seeded with 1, which
// is what every implementation does.
LIBC_INLINE void ensure_seeded() {
  if (seeded)
    return;
  seeded = true;
  seed(state, 1);
}

} // namespace random_internal
} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC_STDLIB_RANDOM_UTIL_H
