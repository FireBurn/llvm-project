//===-- Transliteration for iconv -------------------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// What //TRANSLIT writes a character as when the target set does not have
/// it.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC_ICONV_TRANSLIT_H
#define LLVM_LIBC_SRC_ICONV_TRANSLIT_H

#include "hdr/stdint_proxy.h"
#include "hdr/types/char32_t.h"
#include "hdr/types/size_t.h"
#include "src/__support/macros/attributes.h"
#include "src/__support/macros/config.h"
#include "src/iconv/translit_table.h"

namespace LIBC_NAMESPACE_DECL {
namespace iconv_internal {

// Whether |cp| has a transliteration, and if so its characters. One with no
// characters leaves |cp| out.
LIBC_INLINE bool find_transliteration(char32_t cp, const char16_t *&text,
                                      size_t &length) {
  size_t low = 0;
  size_t high = TRANSLIT_COUNT;
  while (low < high) {
    size_t mid = low + (high - low) / 2;
    if (TRANSLIT_CODE_POINTS[mid] < cp)
      low = mid + 1;
    else
      high = mid;
  }
  if (low < TRANSLIT_COUNT && TRANSLIT_CODE_POINTS[low] == cp) {
    text = TRANSLIT_TEXT + TRANSLIT_OFFSETS[low];
    length = TRANSLIT_OFFSETS[low + 1] - TRANSLIT_OFFSETS[low];
    return true;
  }
  low = 0;
  high = TRANSLIT_REMOVED_COUNT;
  while (low < high) {
    size_t mid = low + (high - low) / 2;
    if (TRANSLIT_REMOVED[2 * mid + 1] < cp)
      low = mid + 1;
    else
      high = mid;
  }
  if (low < TRANSLIT_REMOVED_COUNT && TRANSLIT_REMOVED[2 * low] <= cp) {
    text = nullptr;
    length = 0;
    return true;
  }
  return false;
}

} // namespace iconv_internal
} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC_ICONV_TRANSLIT_H
