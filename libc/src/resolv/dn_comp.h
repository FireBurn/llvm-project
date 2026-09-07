//===-- Implementation header for dn_comp -----------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC_RESOLV_DN_COMP_H
#define LLVM_LIBC_SRC_RESOLV_DN_COMP_H

#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {

int dn_comp(const char *src, unsigned char *dst, int dstsiz,
            unsigned char **dnptrs, unsigned char **lastdnptr);

} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC_RESOLV_DN_COMP_H
