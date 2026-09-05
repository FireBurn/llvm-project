//===-- Implementation of setlocale ---------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/locale/setlocale.h"

#include "hdr/locale_macros.h"
#include "src/__support/CPP/string_view.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/locale/locale.h"

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(char *, setlocale, (int category, const char *locale_name)) {
  if (category < 0 || category > LC_ALL)
    return nullptr;

  // A null name is a question rather than a request to change anything, which
  // is how callers ask what the locale is. It must not be read as a name.
  if (locale_name == nullptr)
    return const_cast<char *>(internal::current_locale_name());

  // Every category is held together rather than one at a time, so a program
  // that sets them separately gets the last one it asked for. What is kept of
  // a locale is the name it goes by and how wide a character is in it.
  return const_cast<char *>(
      internal::set_locale_name(category, cpp::string_view(locale_name)));
}

} // namespace LIBC_NAMESPACE_DECL
