//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation of dgettext_l.
///
//===----------------------------------------------------------------------===//

#include "src/libintl/dgettext_l.h"

#include "hdr/locale_macros.h"
#include "hdr/types/locale_t.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/libintl/catalogs.h"
#include "src/locale/locale.h"

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(char *, dgettext_l,
                   (const char *domainname, const char *msgid,
                    [[maybe_unused]] locale_t loc)) {
  return const_cast<char *>(
      libintl::lookup(domainname, msgid, nullptr, 0, LC_MESSAGES,
                      internal::current_locale_name(LC_MESSAGES)));
}

} // namespace LIBC_NAMESPACE_DECL
