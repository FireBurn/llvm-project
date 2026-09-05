//===-- Implementation of newlocale ---------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/locale/newlocale.h"
#include "hdr/locale_macros.h"
#include "src/__support/CPP/string_view.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/locale/locale.h"

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(locale_t, newlocale,
                   (int category_mask, const char *locale_name, locale_t)) {
  if ((category_mask & ~LC_ALL_MASK) != 0 || locale_name == nullptr)
    return nullptr;

  // There is one locale, so any name it can honour is answered with it.
  // Refusing the names the environment states would leave callers with
  // nothing where they ask for the locale the machine is set to.
  if (!internal::codeset_is_supported(cpp::string_view(locale_name)))
    return nullptr;

  return &c_locale;
}

} // namespace LIBC_NAMESPACE_DECL
