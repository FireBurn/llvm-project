//===-- Implementation of gethostbyaddr -----------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/netdb/gethostbyaddr.h"

#include "hdr/netdb_macros.h"
#include "hdr/sys_socket_macros.h"
#include "hdr/types/struct_sockaddr_in.h"
#include "hdr/types/struct_sockaddr_in6.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/netdb/__h_errno_location.h"
#include "src/netdb/getnameinfo.h"
#include "src/netdb/hostent_storage.h"
#include "src/string/memory_utils/inline_memcpy.h"
#include "src/string/string_utils.h"

namespace LIBC_NAMESPACE_DECL {

// Asks what the host at `addr` is called. getnameinfo already knows how to
// ask that, so this is that answer put into the shape this older interface
// hands back.
LLVM_LIBC_FUNCTION(struct hostent *, gethostbyaddr,
                   (const void *addr, socklen_t len, int type)) {
  if (addr == nullptr) {
    *__h_errno_location() = HOST_NOT_FOUND;
    return nullptr;
  }

  netdb::HostentStorage &storage = netdb::hostent_storage();
  size_t address_size = 0;

  union {
    struct sockaddr_in v4;
    struct sockaddr_in6 v6;
  } socket_address = {};
  socklen_t socket_length = 0;

  if (type == AF_INET && len == 4) {
    address_size = 4;
    socket_address.v4.sin_family = AF_INET;
    inline_memcpy(&socket_address.v4.sin_addr, addr, 4);
    socket_length = static_cast<socklen_t>(sizeof(socket_address.v4));
  } else if (type == AF_INET6 && len == 16) {
    address_size = 16;
    socket_address.v6.sin6_family = AF_INET6;
    inline_memcpy(&socket_address.v6.sin6_addr, addr, 16);
    socket_length = static_cast<socklen_t>(sizeof(socket_address.v6));
  } else {
    *__h_errno_location() = HOST_NOT_FOUND;
    return nullptr;
  }

  // NI_NAMEREQD so that an address with no name is a failure rather than the
  // address written back as though it were one.
  if (getnameinfo(reinterpret_cast<struct sockaddr *>(&socket_address),
                  socket_length, storage.name, sizeof(storage.name), nullptr, 0,
                  NI_NAMEREQD) != 0) {
    *__h_errno_location() = HOST_NOT_FOUND;
    return nullptr;
  }

  inline_memcpy(storage.addresses[0], addr, address_size);
  storage.address_list[0] = reinterpret_cast<char *>(storage.addresses[0]);
  storage.address_list[1] = nullptr;
  storage.aliases[0] = nullptr;

  storage.entry.h_name = storage.name;
  storage.entry.h_aliases = storage.aliases;
  storage.entry.h_addrtype = type;
  storage.entry.h_length = static_cast<int>(address_size);
  storage.entry.h_addr_list = storage.address_list;
  return &storage.entry;
}

} // namespace LIBC_NAMESPACE_DECL
