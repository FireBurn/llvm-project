//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation of setnetgrent
///
//===----------------------------------------------------------------------===//

#include "src/netdb/setnetgrent.h"

#include "hdr/errno_macros.h"
#include "src/__support/CPP/string_view.h"
#include "src/__support/common.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"
#include "src/__support/macros/null_check.h"
#include "src/netdb/netgroup_utils.h"

namespace LIBC_NAMESPACE_DECL {

// Starts reading the triples of a netgroup. Returns 1 if there is such a
// netgroup, and 0 if not.
LLVM_LIBC_FUNCTION(int, setnetgrent, (const char *netgroup)) {
  LIBC_CRASH_ON_NULLPTR(netgroup);
  netgroup::release(netgroup::current());
  return netgroup::load(cpp::string_view(netgroup), netgroup::current()) ? 1
                                                                         : 0;
}

} // namespace LIBC_NAMESPACE_DECL
