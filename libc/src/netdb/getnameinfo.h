//===-- Implementation header for getnameinfo -------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC_NETDB_GETNAMEINFO_H
#define LLVM_LIBC_SRC_NETDB_GETNAMEINFO_H

#include "hdr/types/socklen_t.h"
#include "hdr/types/struct_sockaddr.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {

int getnameinfo(const struct sockaddr *__restrict addr, socklen_t addrlen,
                char *__restrict host, socklen_t hostlen, char *__restrict serv,
                socklen_t servlen, int flags);

} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC_NETDB_GETNAMEINFO_H
