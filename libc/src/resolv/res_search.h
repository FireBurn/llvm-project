//===-- Implementation header of res_search -------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC_RESOLV_RES_SEARCH_H
#define LLVM_LIBC_SRC_RESOLV_RES_SEARCH_H

#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {

int res_search(const char *name, int rr_class, int type, unsigned char *answer,
               int anslen);

} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC_RESOLV_RES_SEARCH_H
