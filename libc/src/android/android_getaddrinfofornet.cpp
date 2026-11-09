//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation of android_getaddrinfofornet.
///
//===----------------------------------------------------------------------===//

#include "src/android/android_getaddrinfofornet.h"
#include "hdr/types/struct_addrinfo.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/netdb/getaddrinfo.h"

namespace LIBC_NAMESPACE_DECL {

// bionic resolves through the DNS settings of a network, chosen by id, and
// marks its sockets. There is one resolver configuration here, so the
// network and the mark select nothing and this is getaddrinfo.
LLVM_LIBC_FUNCTION(int, android_getaddrinfofornet,
                   (const char *nodename, const char *servname,
                    const struct addrinfo *hints, unsigned, unsigned,
                    struct addrinfo **res)) {
  return getaddrinfo(nodename, servname, hints, res);
}

} // namespace LIBC_NAMESPACE_DECL
