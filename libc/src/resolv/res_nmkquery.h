//===-- Implementation header for res_nmkquery ----------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC_RESOLV_RES_NMKQUERY_H
#define LLVM_LIBC_SRC_RESOLV_RES_NMKQUERY_H

#include "src/__support/macros/config.h"

struct __res_state;

namespace LIBC_NAMESPACE_DECL {

int res_nmkquery(struct __res_state *statp, int op, const char *dname,
                 int rr_class, int type, const unsigned char *data, int datalen,
                 const unsigned char *newrr, unsigned char *buf, int buflen);

} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC_RESOLV_RES_NMKQUERY_H
