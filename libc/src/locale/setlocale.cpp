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

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(char *, setlocale, (int category, const char *locale_name)) {
  static char locale_str[] = "C";

  if (category < 0 || category > LC_ALL)
    return nullptr;

  // A null name is a query rather than a request to change anything, which is
  // how callers ask what the current locale is. It must not be parsed.
  if (locale_name == nullptr)
    return locale_str;

  cpp::string_view name(locale_name);
  // The empty string means the locale named by the environment, which here is
  // the only one there is.
  if (!name.empty() && name != "C" && name != "POSIX")
    return nullptr;

  return locale_str;
}

} // namespace LIBC_NAMESPACE_DECL
