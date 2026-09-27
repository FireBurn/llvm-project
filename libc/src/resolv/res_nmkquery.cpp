//===-- Implementation of res_nmkquery ------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/resolv/res_nmkquery.h"

#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/resolv/res_mkquery.h"

namespace LIBC_NAMESPACE_DECL {

// res_mkquery, with the state the caller keeps rather than the thread's own.
LLVM_LIBC_FUNCTION(int, res_nmkquery,
                   (struct __res_state * statp, int op, const char *dname,
                    int rr_class, int type, const unsigned char *data,
                    int datalen, const unsigned char *newrr, unsigned char *buf,
                    int buflen)) {
  return internal::res_mkquery_with(statp, op, dname, rr_class, type, data,
                                    datalen, newrr, buf, buflen);
}

} // namespace LIBC_NAMESPACE_DECL
