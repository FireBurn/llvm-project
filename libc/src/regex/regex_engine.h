//===-- Compiled form and parser for POSIX regular expressions --*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC_REGEX_REGEX_ENGINE_H
#define LLVM_LIBC_SRC_REGEX_REGEX_ENGINE_H

#include "hdr/regex_macros.h"
#include "hdr/stdint_proxy.h"
#include "hdr/types/size_t.h"
#include "src/__support/CPP/optional.h"
#include "src/__support/ctype_utils.h"
#include "src/__support/macros/attributes.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {
namespace regex {

// A parsed pattern is a flat array of nodes referring to each other by index.
// Indices rather than pointers keep the whole compiled form relocatable, so it
// lives in one allocation that regfree releases with a single call.
enum class Op : uint8_t {
  Empty,   // matches the empty string
  Char,    // one literal character
  Any,     // '.'
  Class,   // '[...]'
  Concat,  // left then right
  Alt,     // left or right
  Repeat,  // left, between min and max times
  Group,   // left, capturing as group_index
  Bol,     // '^'
  Eol,     // '$'
  Backref, // '\1' through '\9'
};

// A character class as a bitmap, so membership is a constant time test and
// ranges, named classes and negation all collapse into the same thing.
struct CharClass {
  uint32_t bits[8];

  LIBC_INLINE constexpr void add(unsigned char c) {
    bits[c >> 5] |= 1u << (c & 31);
  }
  LIBC_INLINE constexpr bool has(unsigned char c) const {
    return (bits[c >> 5] >> (c & 31)) & 1u;
  }
  LIBC_INLINE constexpr void negate() {
    for (int i = 0; i < 8; ++i)
      bits[i] = ~bits[i];
  }
};

constexpr int32_t NONE = -1;
// Unbounded repetition, as in '*' or '{2,}'.
constexpr int32_t UNBOUNDED = -1;

struct Node {
  Op op = Op::Empty;
  unsigned char ch = 0;
  int32_t left = NONE;
  int32_t right = NONE;
  int32_t min = 0;
  int32_t max = 0;
  uint32_t index = 0; // group number, class index, or backreference number
};

// The whole compiled pattern. Allocated as one block: this header, then the
// node array, then the class array.
struct Compiled {
  int32_t root = NONE;
  uint32_t node_count = 0;
  uint32_t class_count = 0;
  uint32_t group_count = 0;
  int cflags = 0;
  Node *nodes = nullptr;
  CharClass *classes = nullptr;
};

} // namespace regex
} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC_REGEX_REGEX_ENGINE_H
