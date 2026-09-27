//===-- Implementation of res_nquery --------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/resolv/res_nquery.h"

#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/resolv/res_query.h"

namespace LIBC_NAMESPACE_DECL {

// res_query, with the state the caller keeps rather than the thread's own.
LLVM_LIBC_FUNCTION(int, res_nquery,
                   (struct __res_state * statp, const char *name, int rr_class,
                    int type, unsigned char *answer, int anslen)) {
  return internal::res_query_with(statp, name, rr_class, type, answer, anslen);
}

} // namespace LIBC_NAMESPACE_DECL
