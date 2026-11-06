//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation of fts_open.
///
//===----------------------------------------------------------------------===//

#include "src/fts/fts_open.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/fts/fts_walk.h"

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(FTS *, fts_open, (char *const *paths, int options,
                    __fts_compare_func_t compare)) {
  return fts_internal::open_walk(paths, options, compare);
}

} // namespace LIBC_NAMESPACE_DECL
