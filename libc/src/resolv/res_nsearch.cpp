//===-- Implementation of res_nsearch -------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/resolv/res_nsearch.h"

#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/resolv/res_search.h"

namespace LIBC_NAMESPACE_DECL {

// res_search, with the state the caller keeps rather than the thread's own.
LLVM_LIBC_FUNCTION(int, res_nsearch,
                   (struct __res_state * statp, const char *name, int rr_class,
                    int type, unsigned char *answer, int anslen)) {
  return internal::res_search_with(statp, name, rr_class, type, answer, anslen);
}

} // namespace LIBC_NAMESPACE_DECL
