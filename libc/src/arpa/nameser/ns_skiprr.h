//===-- Implementation header for ns_skiprr ---------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC_ARPA_NAMESER_NS_SKIPRR_H
#define LLVM_LIBC_SRC_ARPA_NAMESER_NS_SKIPRR_H

#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {

int ns_skiprr(const unsigned char *ptr, const unsigned char *eom, int section,
              int count);

} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC_ARPA_NAMESER_NS_SKIPRR_H
