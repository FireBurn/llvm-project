//===-- Implementation of wcsncasecmp -------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/wchar/wcsncasecmp.h"

#include "hdr/types/size_t.h"
#include "hdr/types/wchar_t.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/__support/macros/null_check.h"
#include "src/__support/wctype_utils.h"

namespace LIBC_NAMESPACE_DECL {

// Characters are compared as towlower would return them, so how far case is
// folded follows the wctype mode the library was built with.
LLVM_LIBC_FUNCTION(int, wcsncasecmp,
                   (const wchar_t *left, const wchar_t *right, size_t n)) {
  LIBC_CRASH_ON_NULLPTR(left);
  LIBC_CRASH_ON_NULLPTR(right);

  if (n == 0)
    return 0;

  for (;
       n > 1 && *left && internal::tolower(*left) == internal::tolower(*right);
       --n, ++left, ++right)
    ;

  return internal::tolower(*left) - internal::tolower(*right);
}

} // namespace LIBC_NAMESPACE_DECL
