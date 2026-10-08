//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation of mbrtoc16
///
//===----------------------------------------------------------------------===//

#include "src/uchar/mbrtoc16.h"

#include "hdr/types/char16_t.h"
#include "hdr/types/mbstate_t.h"
#include "hdr/types/size_t.h"
#include "hdr/types/wchar_t.h"
#include "src/__support/common.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"
#include "src/__support/wchar/mbrtowc.h"
#include "src/__support/wchar/mbstate.h"

namespace LIBC_NAMESPACE_DECL {

// A code point above U+FFFF comes out as two calls. The first returns what it
// consumed with the high surrogate; the state keeps the low one, which the
// next call returns as (size_t)-3 without reading anything. Nothing else
// leaves a value in that range in the state, as a decoded character is never
// a surrogate.
LLVM_LIBC_FUNCTION(size_t, mbrtoc16,
                   (char16_t *__restrict pc16, const char *__restrict s,
                    size_t n, mbstate_t *__restrict ps)) {
  static internal::mbstate internal_mbstate;
  auto *state = ps == nullptr ? &internal_mbstate
                              : reinterpret_cast<internal::mbstate *>(ps);

  // A null string asks for the state to be reset, as an empty string does.
  if (s == nullptr) {
    *state = internal::mbstate{};
    return 0;
  }

  if (state->bytes_stored == 0 && state->total_bytes == 0 &&
      state->partial >= 0xDC00 && state->partial <= 0xDFFF) {
    if (pc16 != nullptr)
      *pc16 = static_cast<char16_t>(state->partial);
    state->partial = 0;
    return static_cast<size_t>(-3);
  }

  wchar_t wc;
  auto ret = internal::mbrtowc(&wc, s, n, state);
  if (!ret.has_value()) {
    libc_errno = ret.error();
    return -1;
  }
  if (ret.value() == static_cast<size_t>(-2))
    return ret.value();

  const char32_t c32 = static_cast<char32_t>(wc);
  if (c32 > 0xFFFF) {
    const char32_t v = c32 - 0x10000;
    if (pc16 != nullptr)
      *pc16 = static_cast<char16_t>(0xD800 + (v >> 10));
    state->partial = 0xDC00 + (v & 0x3FF);
  } else if (pc16 != nullptr) {
    *pc16 = static_cast<char16_t>(c32);
  }
  return ret.value();
}

} // namespace LIBC_NAMESPACE_DECL
