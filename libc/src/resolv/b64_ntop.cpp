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
/// Implementation of b64_ntop
///
//===----------------------------------------------------------------------===//

#include "src/resolv/b64_ntop.h"

#include "src/__support/common.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {

namespace {

constexpr char ALPHABET[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

} // anonymous namespace

// Writes the base 64 form of `srclength` bytes, with '=' padding and a
// terminating NUL. Returns the length without the NUL, or -1 if `targsize`
// cannot hold all of it.
LLVM_LIBC_FUNCTION(int, b64_ntop,
                   (const unsigned char *src, size_t srclength, char *target,
                    size_t targsize)) {
  const size_t groups = srclength / 3 + (srclength % 3 != 0);
  if (groups > (static_cast<size_t>(-1) - 1) / 4 || groups * 4 + 1 > targsize)
    return -1;
  // The result has to be representable in the int that is returned.
  if (groups * 4 > static_cast<size_t>(0x7fffffff))
    return -1;

  size_t out = 0;
  size_t i = 0;
  for (; srclength - i >= 3; i += 3) {
    target[out++] = ALPHABET[src[i] >> 2];
    target[out++] = ALPHABET[((src[i] & 0x03) << 4) | (src[i + 1] >> 4)];
    target[out++] = ALPHABET[((src[i + 1] & 0x0f) << 2) | (src[i + 2] >> 6)];
    target[out++] = ALPHABET[src[i + 2] & 0x3f];
  }
  if (srclength - i == 1) {
    target[out++] = ALPHABET[src[i] >> 2];
    target[out++] = ALPHABET[(src[i] & 0x03) << 4];
    target[out++] = '=';
    target[out++] = '=';
  } else if (srclength - i == 2) {
    target[out++] = ALPHABET[src[i] >> 2];
    target[out++] = ALPHABET[((src[i] & 0x03) << 4) | (src[i + 1] >> 4)];
    target[out++] = ALPHABET[(src[i + 1] & 0x0f) << 2];
    target[out++] = '=';
  }
  target[out] = '\0';
  return static_cast<int>(out);
}

} // namespace LIBC_NAMESPACE_DECL
