//===-- Implementation of gethostbyname -----------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/netdb/gethostbyname.h"

#include "hdr/sys_socket_macros.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/netdb/hostent_storage.h"

namespace LIBC_NAMESPACE_DECL {

// The answer is kept in storage the library owns, which the next lookup from
// this thread overwrites. That is what this interface has always been, and
// why getaddrinfo replaced it.
LLVM_LIBC_FUNCTION(struct hostent *, gethostbyname, (const char *name)) {
  return netdb::fill_hostent(name, AF_INET);
}

} // namespace LIBC_NAMESPACE_DECL
