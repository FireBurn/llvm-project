//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation of endnetgrent
///
//===----------------------------------------------------------------------===//

#include "src/netdb/endnetgrent.h"

#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/__support/macros/null_check.h"
#include "src/netdb/netgroup_utils.h"

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(void, endnetgrent, (void)) {
  netgroup::release(netgroup::current());
}

} // namespace LIBC_NAMESPACE_DECL
