//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation of ngettext.
///
//===----------------------------------------------------------------------===//

#include "src/libintl/ngettext.h"

#include "hdr/locale_macros.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/libintl/catalogs.h"
#include "src/locale/locale.h"

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(char *, ngettext,
                   (const char *msgid1, const char *msgid2,
                    unsigned long int n)) {
  return const_cast<char *>(
      libintl::lookup(nullptr, msgid1, msgid2, n, LC_MESSAGES,
                      internal::current_locale_name(LC_MESSAGES)));
}

} // namespace LIBC_NAMESPACE_DECL
