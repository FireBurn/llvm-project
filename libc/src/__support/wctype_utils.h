//===-- Collection of utils for implementing wide char functions --*-C++-*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC___SUPPORT_WCTYPE_UTILS_H
#define LLVM_LIBC_SRC___SUPPORT_WCTYPE_UTILS_H

#include "hdr/types/wchar_t.h"
#include "src/__support/CPP/bit.h"
#include "src/__support/macros/attributes.h" // LIBC_INLINE
#include "src/__support/macros/config.h"
#include "src/__support/wctype_ascii_utils.h"

#define LIBC_WCTYPE_MODE_ASCII 0
#define LIBC_WCTYPE_MODE_UTF8 1

#ifndef LIBC_CONF_WCTYPE_MODE
#define LIBC_CONF_WCTYPE_MODE LIBC_WCTYPE_MODE_ASCII
#endif

#if LIBC_CONF_WCTYPE_MODE == LIBC_WCTYPE_MODE_UTF8
#include "src/__support/wctype/wctype_classification_utils.h"
#include "src/__support/wctype/wctype_conversion_utils.h"
#endif

namespace LIBC_NAMESPACE_DECL {
namespace internal {

LIBC_INLINE constexpr bool islower(wchar_t wch) {
#if LIBC_CONF_WCTYPE_MODE != LIBC_WCTYPE_MODE_UTF8
  return ascii::islower(wch);
#else
  if (static_cast<uint32_t>(wch) < 128) {
    return ascii::islower(wch);
  }
  return lookup_properties(wch) & PropertyFlag::LOWER;
#endif
}

LIBC_INLINE constexpr bool isupper(wchar_t wch) {
#if LIBC_CONF_WCTYPE_MODE != LIBC_WCTYPE_MODE_UTF8
  return ascii::isupper(wch);
#else
  if (static_cast<uint32_t>(wch) < 128) {
    return ascii::isupper(wch);
  }
  return lookup_properties(wch) & PropertyFlag::UPPER;
#endif
}

LIBC_INLINE constexpr bool isdigit(wchar_t wch) {
  // In C.UT8, only ASCII digits are considered digits
  return ascii::isdigit(wch);
}

LIBC_INLINE constexpr bool isalpha(wchar_t wch) {
#if LIBC_CONF_WCTYPE_MODE != LIBC_WCTYPE_MODE_UTF8
  return ascii::isalpha(wch);
#else
  if (static_cast<uint32_t>(wch) < 128) {
    return ascii::isalpha(wch);
  }
  return lookup_properties(wch) & PropertyFlag::ALPHA;
#endif
}

LIBC_INLINE constexpr bool isalnum(wchar_t wch) {
#if LIBC_CONF_WCTYPE_MODE != LIBC_WCTYPE_MODE_UTF8
  return ascii::isalnum(wch);
#else
  if (static_cast<uint32_t>(wch) < 128) {
    return ascii::isalnum(wch);
  }
  // Only need to check ALPHA, digit cases are covered by ASCII path
  return lookup_properties(wch) & PropertyFlag::ALPHA;
#endif
}

LIBC_INLINE constexpr bool isspace(wchar_t wch) {
#if LIBC_CONF_WCTYPE_MODE != LIBC_WCTYPE_MODE_UTF8
  return ascii::isspace(wch);
#else
  if (static_cast<uint32_t>(wch) < 128) {
    return ascii::isspace(wch);
  }
  return lookup_properties(wch) & PropertyFlag::SPACE;
#endif
}

LIBC_INLINE constexpr bool isblank(wchar_t wch) {
#if LIBC_CONF_WCTYPE_MODE != LIBC_WCTYPE_MODE_UTF8
  return ascii::isblank(wch);
#else
  if (static_cast<uint32_t>(wch) < 128) {
    return ascii::isblank(wch);
  }
  return lookup_properties(wch) & PropertyFlag::BLANK;
#endif
}

LIBC_INLINE constexpr bool isgraph(wchar_t wch) {
#if LIBC_CONF_WCTYPE_MODE != LIBC_WCTYPE_MODE_UTF8
  return ascii::isgraph(wch);
#else
  if (static_cast<uint32_t>(wch) < 128) {
    return ascii::isgraph(wch);
  }
  // print && !space
  return (lookup_properties(wch) &
          (PropertyFlag::PRINT | PropertyFlag::SPACE)) == PropertyFlag::PRINT;
#endif
}

LIBC_INLINE constexpr bool isprint(wchar_t wch) {
#if LIBC_CONF_WCTYPE_MODE != LIBC_WCTYPE_MODE_UTF8
  return ascii::isprint(wch);
#else
  if (static_cast<uint32_t>(wch) < 128) {
    return ascii::isprint(wch);
  }
  return lookup_properties(wch) & PropertyFlag::PRINT;
#endif
}
LIBC_INLINE constexpr bool isxdigit(wchar_t wch) {
  // Hexadecimal digits are the same in C.UTF8 as in ASCII
  return ascii::isxdigit(wch);
}

LIBC_INLINE constexpr bool iscntrl(wchar_t wch) {
#if LIBC_CONF_WCTYPE_MODE != LIBC_WCTYPE_MODE_UTF8
  return ascii::iscntrl(wch);
#else
  if (static_cast<uint32_t>(wch) < 128) {
    return ascii::iscntrl(wch);
  }
  return lookup_properties(wch) & PropertyFlag::CNTRL;
#endif
}

LIBC_INLINE constexpr bool ispunct(wchar_t wch) {
#if LIBC_CONF_WCTYPE_MODE != LIBC_WCTYPE_MODE_UTF8
  return ascii::ispunct(wch);
#else
  if (static_cast<uint32_t>(wch) < 128) {
    return ascii::ispunct(wch);
  }
  return lookup_properties(wch) & PropertyFlag::PUNCT;
#endif
}

LIBC_INLINE constexpr wchar_t tolower(wchar_t wch) {
#if LIBC_CONF_WCTYPE_MODE != LIBC_WCTYPE_MODE_UTF8
  return ascii::tolower(wch);
#else
  if (static_cast<uint32_t>(wch) < 128) {
    return ascii::tolower(wch);
  }
  return static_cast<wchar_t>(wctype_internal::tolower(wch));
#endif
}

LIBC_INLINE constexpr wchar_t toupper(wchar_t wch) {
#if LIBC_CONF_WCTYPE_MODE != LIBC_WCTYPE_MODE_UTF8
  return ascii::toupper(wch);
#else
  if (static_cast<uint32_t>(wch) < 128) {
    return ascii::toupper(wch);
  }
  return static_cast<wchar_t>(wctype_internal::toupper(wch));
#endif
}

} // namespace internal
} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC___SUPPORT_WCTYPE_UTILS_H
