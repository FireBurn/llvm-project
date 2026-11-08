//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
///
/// \file
/// Implementation of b64_pton
///
//===----------------------------------------------------------------------===//

#include "src/resolv/b64_pton.h"

#include "src/__support/common.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {

namespace {

constexpr int DIGIT_NONE = -1;

constexpr int digit_of(char c) {
  if (c >= 'A' && c <= 'Z')
    return c - 'A';
  if (c >= 'a' && c <= 'z')
    return c - 'a' + 26;
  if (c >= '0' && c <= '9')
    return c - '0' + 52;
  if (c == '+')
    return 62;
  if (c == '/')
    return 63;
  return DIGIT_NONE;
}

constexpr bool is_space(char c) {
  return c == ' ' || (c >= '\t' && c <= '\r');
}

} // anonymous namespace

// Reads base 64 text, skipping white space, into at most `targsize` bytes.
// Returns how many bytes it made, or -1 for a character outside the alphabet,
// padding that is wrong or followed by anything but white space, or output that
// does not fit. A null `target` only counts. The bits left over in a final
// partial group have to be zero, as the padding is meant to say.
LLVM_LIBC_FUNCTION(int, b64_pton,
                   (const char *src, unsigned char *target, size_t targsize)) {
  size_t out = 0;
  int state = 0;
  unsigned leftover = 0;
  char ch = '\0';

  auto emit = [&](unsigned byte) {
    if (target != nullptr) {
      if (out >= targsize)
        return false;
      target[out] = static_cast<unsigned char>(byte);
    }
    ++out;
    return true;
  };

  while ((ch = *src++) != '\0') {
    if (is_space(ch))
      continue;
    if (ch == '=')
      break;
    const int digit = digit_of(ch);
    if (digit == DIGIT_NONE)
      return -1;
    const unsigned d = static_cast<unsigned>(digit);
    switch (state) {
    case 0:
      leftover = d << 2;
      state = 1;
      break;
    case 1:
      if (!emit(leftover | (d >> 4)))
        return -1;
      leftover = (d & 0x0f) << 4;
      state = 2;
      break;
    case 2:
      if (!emit(leftover | (d >> 2)))
        return -1;
      leftover = (d & 0x03) << 6;
      state = 3;
      break;
    default:
      if (!emit(leftover | d))
        return -1;
      leftover = 0;
      state = 0;
      break;
    }
  }

  if (ch == '=') {
    ch = *src++;
    switch (state) {
    case 0:
    case 1:
      return -1;
    case 2:
      // Two characters of a group need a second '=', white space allowed
      // between them.
      while (ch != '\0' && is_space(ch))
        ch = *src++;
      if (ch != '=')
        return -1;
      ch = *src++;
      [[fallthrough]];
    default:
      for (; ch != '\0'; ch = *src++)
        if (!is_space(ch))
          return -1;
      if (leftover != 0)
        return -1;
    }
  } else if (state != 0) {
    return -1;
  }
  return static_cast<int>(out);
}

} // namespace LIBC_NAMESPACE_DECL
