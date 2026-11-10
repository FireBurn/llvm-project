//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation header for ns_name_rollback
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC_ARPA_NAMESER_NS_NAME_ROLLBACK_H
#define LLVM_LIBC_SRC_ARPA_NAMESER_NS_NAME_ROLLBACK_H

#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {

void ns_name_rollback(const unsigned char *src, const unsigned char **dnptrs,
                      const unsigned char **lastdnptr);

} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC_ARPA_NAMESER_NS_NAME_ROLLBACK_H
