//===-- Implementation header for ns_put32 ----------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC_ARPA_NAMESER_NS_PUT32_H
#define LLVM_LIBC_SRC_ARPA_NAMESER_NS_PUT32_H

#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {

void ns_put32(unsigned long src, unsigned char *dst);

} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC_ARPA_NAMESER_NS_PUT32_H
