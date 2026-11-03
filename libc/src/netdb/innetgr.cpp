//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation of innetgr
///
//===----------------------------------------------------------------------===//

#include "src/netdb/innetgr.h"

#include "src/__support/CPP/string_view.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/__support/macros/null_check.h"
#include "src/netdb/netgroup_utils.h"

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(int, innetgr,
                   (const char *netgroup, const char *host, const char *user,
                    const char *domain)) {
  LIBC_CRASH_ON_NULLPTR(netgroup);
  netgroup::Group group;
  if (!netgroup::load(cpp::string_view(netgroup), group))
    return 0;
  const bool found = netgroup::contains(group, host, user, domain);
  netgroup::release(group);
  return found ? 1 : 0;
}

} // namespace LIBC_NAMESPACE_DECL
