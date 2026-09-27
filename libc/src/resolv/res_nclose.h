//===-- Implementation header for res_nclose ------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC_RESOLV_RES_NCLOSE_H
#define LLVM_LIBC_SRC_RESOLV_RES_NCLOSE_H

#include "src/__support/macros/config.h"

struct __res_state;

namespace LIBC_NAMESPACE_DECL {

void res_nclose(struct __res_state *statp);

} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC_RESOLV_RES_NCLOSE_H
