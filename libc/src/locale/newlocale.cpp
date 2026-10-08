//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation of newlocale.
///
//===----------------------------------------------------------------------===//

#include "src/locale/newlocale.h"
#include "hdr/locale_macros.h"
#include "src/__support/CPP/string_view.h"
#include "src/__support/common.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"
#include "src/locale/locale.h"

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(locale_t, newlocale,
                   (int category_mask, const char *locale_name, locale_t)) {
  if (!locale_name || (category_mask & ~LC_ALL_MASK) != 0) {
    libc_errno = EINVAL;
    return nullptr;
  }

  cpp::string_view name(locale_name);
  if (name.empty())
    return DEFAULT_LOCALE_IS_UTF8 ? &utf8_locale : &c_locale;

  if (is_c_locale_name(name))
    return &c_locale;

  // Any name whose character set can be honoured is answered with the locale
  // that has that character set. Refusing the names the environment states
  // would leave callers with nothing where they ask for the machine's locale.
  if (internal::locale_name_is_supported(name))
    return internal::name_states_utf8(name) ? &utf8_locale : &c_locale;

  libc_errno = ENOENT;
  return nullptr;
}

} // namespace LIBC_NAMESPACE_DECL
