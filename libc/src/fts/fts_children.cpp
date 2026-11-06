//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation of fts_children.
///
//===----------------------------------------------------------------------===//

#include "src/fts/fts_children.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/fts/fts_walk.h"

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(FTSENT *, fts_children, (FTS *fts, int options)) {
  return fts_internal::children_of(fts, options);
}

} // namespace LIBC_NAMESPACE_DECL
