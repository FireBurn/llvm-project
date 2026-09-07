//===-- Implementation header for dn_expand ---------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC_RESOLV_DN_EXPAND_H
#define LLVM_LIBC_SRC_RESOLV_DN_EXPAND_H

#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {

int dn_expand(const unsigned char *msg, const unsigned char *eom,
              const unsigned char *src, char *dn, int dnsiz);

} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC_RESOLV_DN_EXPAND_H
