//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation of c16rtomb
///
//===----------------------------------------------------------------------===//

#include "src/uchar/c16rtomb.h"

#include "hdr/errno_macros.h"
#include "hdr/types/char16_t.h"
#include "hdr/types/mbstate_t.h"
#include "hdr/types/size_t.h"
#include "hdr/types/wchar_t.h"
#include "src/__support/common.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"
#include "src/__support/wchar/mbstate.h"
#include "src/__support/wchar/wcrtomb.h"

namespace LIBC_NAMESPACE_DECL {

// A high surrogate is held in the state and writes nothing; the low surrogate
// that follows it completes the code point.
LLVM_LIBC_FUNCTION(size_t, c16rtomb,
                   (char *__restrict s, char16_t c16,
                    mbstate_t *__restrict ps)) {
  static internal::mbstate internal_mbstate;
  auto *state = ps == nullptr ? &internal_mbstate
                              : reinterpret_cast<internal::mbstate *>(ps);

  const bool pending = state->bytes_stored == 0 && state->total_bytes == 0 &&
                       state->partial >= 0xD800 && state->partial <= 0xDBFF;

  // A null buffer asks what a null character would take, which is one byte.
  char buf[sizeof(char32_t)];
  if (s == nullptr) {
    s = buf;
    c16 = u'\0';
  }

  char32_t c32 = c16;
  const bool is_low = c16 >= 0xDC00 && c16 <= 0xDFFF;
  const bool is_high = c16 >= 0xD800 && c16 <= 0xDBFF;
  if (pending) {
    const char32_t high = state->partial;
    state->partial = 0;
    if (!is_low) {
      libc_errno = EILSEQ;
      return -1;
    }
    c32 = 0x10000 + ((high - 0xD800) << 10) + (c16 - 0xDC00);
  } else if (is_high) {
    state->partial = c16;
    return 0;
  } else if (is_low) {
    libc_errno = EILSEQ;
    return -1;
  }

  internal::mbstate out;
  auto result = internal::wcrtomb(s, static_cast<wchar_t>(c32), &out);
  if (!result.has_value()) {
    libc_errno = result.error();
    return -1;
  }
  return result.value();
}

} // namespace LIBC_NAMESPACE_DECL
