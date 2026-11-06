//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation of fts_read.
///
//===----------------------------------------------------------------------===//

#include "src/fts/fts_read.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/fts/fts_walk.h"

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(FTSENT *, fts_read, (FTS *fts)) {
  return fts_internal::read_next(fts);
}

} // namespace LIBC_NAMESPACE_DECL
