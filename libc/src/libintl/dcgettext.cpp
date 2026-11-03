//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation of dcgettext.
///
//===----------------------------------------------------------------------===//

#include "src/libintl/dcgettext.h"

#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/libintl/catalogs.h"
#include "src/locale/locale.h"

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(char *, dcgettext,
                   (const char *domainname, const char *msgid, int category)) {
  return const_cast<char *>(
      libintl::lookup(domainname, msgid, nullptr, 0, category,
                      internal::current_locale_name(category)));
}

} // namespace LIBC_NAMESPACE_DECL
