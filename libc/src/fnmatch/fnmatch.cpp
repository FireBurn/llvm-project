//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// This file contains the implementation of the fnmatch function.
///
//===----------------------------------------------------------------------===//

#include "src/fnmatch/fnmatch.h"
#include "hdr/fnmatch_macros.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/fnmatch/fnmatch_matcher.h"

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(int, fnmatch,
                   (const char *pattern, const char *string, int flags)) {
  if (pattern == nullptr || string == nullptr)
    return FNM_NOMATCH;
  fnmatch_internal::Matcher matcher(pattern, string, flags);
  return matcher.run() ? 0 : FNM_NOMATCH;
}

} // namespace LIBC_NAMESPACE_DECL
