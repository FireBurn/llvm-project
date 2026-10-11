//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Puts a test in a UTF-8 locale
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_TEST_UNITTEST_UTF8LOCALE_H
#define LLVM_LIBC_TEST_UNITTEST_UTF8LOCALE_H

#include "hdr/locale_macros.h"
#include "src/__support/macros/config.h"
#include "src/locale/locale.h"

namespace LIBC_NAMESPACE_DECL {
namespace testing {

// The multibyte conversions follow LC_CTYPE, which is the C locale, and so
// ASCII only, until something asks for another. A test of what they do with
// UTF-8 includes this header to say so for the whole of the program.
struct Utf8Locale {
  Utf8Locale() {
    LIBC_NAMESPACE::internal::set_locale_name(LC_CTYPE, "C.UTF-8");
  }
};

static Utf8Locale utf8_locale;

} // namespace testing
} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_TEST_UNITTEST_UTF8LOCALE_H
