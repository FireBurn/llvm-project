//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation of bindtextdomain.
///
//===----------------------------------------------------------------------===//

#include "src/libintl/bindtextdomain.h"

#include "hdr/errno_macros.h"
#include "src/__support/common.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"
#include "src/libintl/catalogs.h"

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(char *, bindtextdomain,
                   (const char *domainname, const char *dirname)) {
  if (domainname == nullptr || domainname[0] == '\0')
    return nullptr;
  if (dirname == nullptr)
    return const_cast<char *>(libintl::domain_directory(domainname));
  const char *directory = libintl::set_domain_directory(domainname, dirname);
  if (directory == nullptr)
    libc_errno = ENOMEM;
  return const_cast<char *>(directory);
}

} // namespace LIBC_NAMESPACE_DECL
