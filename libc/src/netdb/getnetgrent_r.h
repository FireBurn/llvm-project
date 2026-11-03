//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation header for getnetgrent_r
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC_NETDB_GETNETGRENT_R_H
#define LLVM_LIBC_SRC_NETDB_GETNETGRENT_R_H

#include "hdr/types/size_t.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {

int getnetgrent_r(char **host, char **user, char **domain, char *buf,
                  size_t buflen);

} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC_NETDB_GETNETGRENT_R_H
