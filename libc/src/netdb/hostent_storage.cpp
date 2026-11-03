//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Where the older host lookups keep their answer
///
//===----------------------------------------------------------------------===//

#include "src/netdb/hostent_storage.h"

#include "hdr/types/socklen_t.h"
#include "hdr/types/struct_sockaddr_in.h"
#include "hdr/types/struct_sockaddr_in6.h"
#include "src/netdb/getnameinfo_utils.h"

#include "hdr/netdb_macros.h"
#include "hdr/sys_socket_macros.h"
#include "src/__support/macros/attributes.h"
#include "src/__support/macros/config.h"
#include "src/netdb/__h_errno_location.h"
#include "src/string/memory_utils/inline_memcpy.h"
#include "src/string/string_utils.h"

namespace LIBC_NAMESPACE_DECL {
namespace netdb {

namespace {

LIBC_THREAD_LOCAL HostentStorage storage;

} // anonymous namespace

HostentStorage &hostent_storage() { return storage; }

struct hostent *fill_hostent(const char *name, int family) {
  int &error = *internal::h_errno_location_impl();

  if (name == nullptr) {
    error = HOST_NOT_FOUND;
    return nullptr;
  }
  // These lookups report one kind of address at a time, since the caller is
  // told only one length for all of them.
  if (family != AF_INET && family != AF_INET6) {
    error = NO_RECOVERY;
    return nullptr;
  }

  resolv::Address found[resolv::MAX_ADDRESSES];
  char canonical[resolv::MAX_NAME + 1];
  const int count = resolv::lookup_name(name, family, 0, found,
                                        resolv::MAX_ADDRESSES, canonical);
  if (count <= 0) {
    // The codes these report are their own, not getaddrinfo's.
    error = count == EAI_AGAIN ? TRY_AGAIN : HOST_NOT_FOUND;
    return nullptr;
  }

  const char *official = canonical[0] != '\0' ? canonical : name;
  const size_t length = internal::string_length(official);
  if (length > resolv::MAX_NAME) {
    error = NO_RECOVERY;
    return nullptr;
  }
  inline_memcpy(storage.name, official, length + 1);

  const size_t size = family == AF_INET ? 4 : 16;
  size_t kept = 0;
  for (int i = 0; i < count; ++i) {
    if (found[i].family != family)
      continue;
    inline_memcpy(storage.addresses[kept], found[i].bytes, size);
    storage.address_list[kept] =
        reinterpret_cast<char *>(storage.addresses[kept]);
    ++kept;
  }
  if (kept == 0) {
    error = NO_DATA;
    return nullptr;
  }
  storage.address_list[kept] = nullptr;
  storage.aliases[0] = nullptr;

  storage.entry.h_name = storage.name;
  storage.entry.h_aliases = storage.aliases;
  storage.entry.h_addrtype = family;
  storage.entry.h_length = static_cast<int>(size);
  storage.entry.h_addr_list = storage.address_list;
  error = 0;
  return &storage.entry;
}

struct hostent *fill_hostent_by_address(const void *addr, socklen_t len,
                                        int type) {
  if (addr == nullptr) {
    *internal::h_errno_location_impl() = HOST_NOT_FOUND;
    return nullptr;
  }

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
    *internal::h_errno_location_impl() = HOST_NOT_FOUND;
    return nullptr;
  }

  // NI_NAMEREQD so that an address with no name is a failure rather than the
  // address written back as though it were one.
  if (internal::getnameinfo_impl(
          reinterpret_cast<struct sockaddr *>(&socket_address), socket_length,
          storage.name, sizeof(storage.name), nullptr, 0, NI_NAMEREQD) != 0) {
    *internal::h_errno_location_impl() = HOST_NOT_FOUND;
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

} // namespace netdb
} // namespace LIBC_NAMESPACE_DECL
