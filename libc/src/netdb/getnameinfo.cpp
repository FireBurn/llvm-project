//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation of getnameinfo
///
//===----------------------------------------------------------------------===//

#include "src/netdb/getnameinfo.h"

#include "hdr/types/socklen_t.h"
#include "hdr/types/struct_sockaddr.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/netdb/getnameinfo_utils.h"

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(int, getnameinfo,
                   (const struct sockaddr *__restrict addr, socklen_t addrlen,
                    char *__restrict host, socklen_t hostlen,
                    char *__restrict serv, socklen_t servlen, int flags)) {
  return internal::getnameinfo_impl(addr, addrlen, host, hostlen, serv, servlen,
                                    flags);
}

} // namespace LIBC_NAMESPACE_DECL
