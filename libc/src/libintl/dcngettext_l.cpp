//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation of dcngettext_l.
///
//===----------------------------------------------------------------------===//

#include "src/libintl/dcngettext_l.h"

#include "hdr/types/locale_t.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/libintl/catalogs.h"
#include "src/locale/locale.h"

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(char *, dcngettext_l,
                   (const char *domainname, const char *msgid1,
                    const char *msgid2, unsigned long int n, int category,
                    [[maybe_unused]] locale_t loc)) {
  return const_cast<char *>(
      libintl::lookup(domainname, msgid1, msgid2, n, category,
                      internal::current_locale_name(category)));
}

} // namespace LIBC_NAMESPACE_DECL
