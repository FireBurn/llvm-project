//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation of __nss_configure_lookup.
///
//===----------------------------------------------------------------------===//

#include "src/nss/__nss_configure_lookup.h"

#include "hdr/errno_macros.h"
#include "src/__support/CPP/string_view.h"
#include "src/__support/common.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"
#include "src/__support/macros/null_check.h"
#include "src/__support/nss/config.h"

namespace LIBC_NAMESPACE_DECL {

// Replaces the services a database is asked for the rest of the process, as
// the line of nsswitch.conf would, and keeps them when the file changes.
LLVM_LIBC_FUNCTION(int, __nss_configure_lookup,
                   (const char *database, const char *services)) {
  LIBC_CRASH_ON_NULLPTR(database);
  LIBC_CRASH_ON_NULLPTR(services);
  if (!nss::configure(database, services)) {
    libc_errno = EINVAL;
    return -1;
  }
  return 0;
}

} // namespace LIBC_NAMESPACE_DECL
