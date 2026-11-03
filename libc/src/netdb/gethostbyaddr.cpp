//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation of gethostbyaddr
///
//===----------------------------------------------------------------------===//

#include "src/netdb/gethostbyaddr.h"

#include "hdr/netdb_macros.h"
#include "hdr/sys_socket_macros.h"
#include "hdr/types/struct_sockaddr_in.h"
#include "hdr/types/struct_sockaddr_in6.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/netdb/__h_errno_location.h"
#include "src/netdb/hostent_storage.h"
#include "src/string/memory_utils/inline_memcpy.h"
#include "src/string/string_utils.h"

namespace LIBC_NAMESPACE_DECL {

// Asks what the host at `addr` is called. getnameinfo already knows how to
// ask that, so this is that answer put into the shape this older interface
// hands back.
LLVM_LIBC_FUNCTION(struct hostent *, gethostbyaddr,
                   (const void *addr, socklen_t len, int type)) {
  return netdb::fill_hostent_by_address(addr, len, type);
}

} // namespace LIBC_NAMESPACE_DECL
