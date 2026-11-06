//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation of fts_set.
///
//===----------------------------------------------------------------------===//

#include "src/fts/fts_set.h"
#include "hdr/errno_macros.h"
#include "hdr/fts_macros.h"
#include "src/__support/libc_errno.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(int, fts_set, (FTS *fts, FTSENT *entry, int options)) {
  (void)fts;
  switch (options) {
  case FTS_AGAIN:
  case FTS_FOLLOW:
  case FTS_NOINSTR:
  case FTS_SKIP:
    entry->fts_instr = static_cast<unsigned short>(options);
    return 0;
  default:
    libc_errno = EINVAL;
    return 1;
  }
}

} // namespace LIBC_NAMESPACE_DECL
