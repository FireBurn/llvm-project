//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation of getnetgrent
///
//===----------------------------------------------------------------------===//

#include "src/netdb/getnetgrent.h"

#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/__support/macros/null_check.h"
#include "src/netdb/netgroup_utils.h"

namespace LIBC_NAMESPACE_DECL {

// The strings stay valid until the next call of setnetgrent or endnetgrent.
LLVM_LIBC_FUNCTION(int, getnetgrent,
                   (char **host, char **user, char **domain)) {
  LIBC_CRASH_ON_NULLPTR(host);
  LIBC_CRASH_ON_NULLPTR(user);
  LIBC_CRASH_ON_NULLPTR(domain);
  netgroup::Triple triple;
  if (!netgroup::next(netgroup::current(), triple))
    return 0;
  *host = const_cast<char *>(triple.host);
  *user = const_cast<char *>(triple.user);
  *domain = const_cast<char *>(triple.domain);
  return 1;
}

} // namespace LIBC_NAMESPACE_DECL
