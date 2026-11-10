//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation header for ns_name_skip
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC_ARPA_NAMESER_NS_NAME_SKIP_H
#define LLVM_LIBC_SRC_ARPA_NAMESER_NS_NAME_SKIP_H

#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {

int ns_name_skip(const unsigned char **ptr, const unsigned char *eom);

} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC_ARPA_NAMESER_NS_NAME_SKIP_H
