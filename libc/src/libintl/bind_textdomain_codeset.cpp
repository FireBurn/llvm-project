//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation of bind_textdomain_codeset.
///
//===----------------------------------------------------------------------===//

#include "src/libintl/bind_textdomain_codeset.h"

#include "hdr/errno_macros.h"
#include "src/__support/common.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"
#include "src/libintl/catalogs.h"

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(char *, bind_textdomain_codeset,
                   (const char *domainname, const char *codeset)) {
  if (domainname == nullptr || domainname[0] == '\0')
    return nullptr;
  if (codeset == nullptr)
    return const_cast<char *>(libintl::domain_codeset(domainname));
  const char *stored = libintl::set_domain_codeset(domainname, codeset);
  if (stored == nullptr)
    libc_errno = ENOMEM;
  return const_cast<char *>(stored);
}

} // namespace LIBC_NAMESPACE_DECL
