//===-- Implementation of getaddrinfo -------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/netdb/getaddrinfo.h"
#include "src/netdb/freeaddrinfo.h"

#include "hdr/errno_macros.h"
#include "hdr/func/free.h"
#include "hdr/func/malloc.h"
#include "hdr/netdb_macros.h"
#include "hdr/sys_socket_macros.h"
#include "hdr/types/struct_addrinfo.h"
#include "hdr/types/struct_sockaddr_in.h"
#include "hdr/types/struct_sockaddr_in6.h"
#include "include/llvm-libc-macros/netinet-in-macros.h"
#include "src/__support/common.h"
#include "src/__support/ctype_utils.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"
#include "src/__support/str_to_integer.h"
#include "src/netdb/resolv/lookup.h"
#include "src/netdb/resolv/services.h"
#include "src/string/memory_utils/inline_memcpy.h"
#include "src/string/string_utils.h"

namespace LIBC_NAMESPACE_DECL {

namespace {

// What one answer takes: the record itself, the address it points at, and
// the name where there is one. They are allocated together so that
// freeaddrinfo has one thing to give back for each.
struct Entry {
  struct addrinfo info;
  union {
    struct sockaddr_in v4;
    struct sockaddr_in6 v6;
  } address;
};

// Turns the service into a port, in the order the wire uses.
int resolve_service(const char *servname, int flags, int socktype,
                    uint16_t &port) {
  port = 0;
  if (servname == nullptr || servname[0] == '\0')
    return 0;

  // A service written as a number is that number, whatever any file says.
  bool numeric = true;
  for (const char *p = servname; *p != '\0'; ++p)
    if (!internal::isdigit(*p)) {
      numeric = false;
      break;
    }
  if (numeric) {
    auto value = internal::strtointeger<long>(servname, 10);
    if (value.value < 0 || value.value > 65535)
      return EAI_SERVICE;
    const uint16_t host_order = static_cast<uint16_t>(value.value);
    port = static_cast<uint16_t>((host_order << 8) | (host_order >> 8));
    return 0;
  }
  if ((flags & AI_NUMERICSERV) != 0)
    return EAI_NONAME;

  const char *protocol = nullptr;
  if (socktype == SOCK_DGRAM)
    protocol = "udp";
  else if (socktype == SOCK_STREAM)
    protocol = "tcp";

  if (!resolv::port_for_service(servname, protocol, port))
    return EAI_SERVICE;
  return 0;
}

// The kinds of socket an answer is given for when the caller did not say.
struct SocketKind {
  int socktype;
  int protocol;
};

constexpr SocketKind DEFAULT_KINDS[] = {
    {SOCK_STREAM, IPPROTO_TCP}, {SOCK_DGRAM, IPPROTO_UDP}, {SOCK_RAW, 0}};

} // anonymous namespace

LLVM_LIBC_FUNCTION(int, getaddrinfo,
                   (const char *__restrict nodename,
                    const char *__restrict servname,
                    const struct addrinfo *__restrict hints,
                    struct addrinfo **__restrict res)) {
  if (res == nullptr)
    return EAI_SYSTEM;
  *res = nullptr;
  if (nodename == nullptr && servname == nullptr)
    return EAI_NONAME;

  int flags = 0;
  int family = AF_UNSPEC;
  int socktype = 0;
  int protocol = 0;
  if (hints != nullptr) {
    flags = hints->ai_flags;
    family = hints->ai_family;
    socktype = hints->ai_socktype;
    protocol = hints->ai_protocol;
    if (family != AF_UNSPEC && family != AF_INET && family != AF_INET6)
      return EAI_FAMILY;
    if (socktype != 0 && socktype != SOCK_STREAM && socktype != SOCK_DGRAM &&
        socktype != SOCK_RAW)
      return EAI_SOCKTYPE;
  }

  uint16_t port = 0;
  if (const int error = resolve_service(servname, flags, socktype, port))
    return error;

  resolv::Address found[resolv::MAX_ADDRESSES];
  size_t count = 0;
  char canonical_name[resolv::MAX_NAME + 1];
  canonical_name[0] = '\0';

  if (nodename == nullptr || nodename[0] == '\0') {
    // No name means the machine this is running on: where the caller means to
    // listen, that is any address it has, and otherwise it is the one that
    // comes back to here.
    const bool passive = (flags & AI_PASSIVE) != 0;
    // Somewhere to listen is reported with the older kind first, and
    // somewhere to connect back to here with the newer, which is what the
    // rules for choosing between them come to in each case.
    const int order[2] = {passive ? AF_INET : AF_INET6,
                          passive ? AF_INET6 : AF_INET};
    for (int wanted : order) {
      if (family != AF_UNSPEC && family != wanted)
        continue;
      resolv::Address &entry = found[count++];
      entry.family = wanted;
      entry.scope = 0;
      for (size_t i = 0; i < 16; ++i)
        entry.bytes[i] = 0;
      if (wanted == AF_INET6) {
        if (!passive)
          entry.bytes[15] = 1;
      } else if (!passive) {
        entry.bytes[0] = 127;
        entry.bytes[3] = 1;
      }
    }
  } else {
    const int looked = resolv::lookup_name(
        nodename, family, flags, found, resolv::MAX_ADDRESSES, canonical_name);
    // The EAI_ codes are negative, so anything below zero is one of them.
    if (looked < 0)
      return looked;
    count = static_cast<size_t>(looked);
  }
  if (count == 0)
    return EAI_NONAME;

  // Which kinds of socket to report each address for.
  const SocketKind *kinds = DEFAULT_KINDS;
  // A raw socket is only reported where the caller asked for no particular
  // protocol, since it is the protocol that says what a raw socket carries.
  size_t kind_count = protocol == 0 ? 3 : 2;
  SocketKind asked;
  if (socktype != 0 || protocol != 0) {
    asked.socktype = socktype;
    asked.protocol = protocol;
    if (asked.socktype == 0)
      asked.socktype = protocol == IPPROTO_UDP ? SOCK_DGRAM : SOCK_STREAM;
    if (asked.protocol == 0)
      asked.protocol = socktype == SOCK_DGRAM ? IPPROTO_UDP : IPPROTO_TCP;
    kinds = &asked;
    kind_count = 1;
  }

  struct addrinfo *head = nullptr;
  struct addrinfo *tail = nullptr;
  char *canonical = nullptr;

  for (size_t i = 0; i < count; ++i) {
    for (size_t k = 0; k < kind_count; ++k) {
      auto *entry = static_cast<Entry *>(::malloc(sizeof(Entry)));
      if (entry == nullptr) {
        LIBC_NAMESPACE::freeaddrinfo(head);
        ::free(canonical);
        return EAI_MEMORY;
      }
      entry->info.ai_flags = 0;
      entry->info.ai_family = found[i].family;
      entry->info.ai_socktype = kinds[k].socktype;
      entry->info.ai_protocol = kinds[k].protocol;
      entry->info.ai_canonname = nullptr;
      entry->info.ai_next = nullptr;
      entry->info.ai_addr =
          reinterpret_cast<struct sockaddr *>(&entry->address);

      if (found[i].family == AF_INET) {
        entry->address.v4 = {};
        entry->address.v4.sin_family = AF_INET;
        entry->address.v4.sin_port = port;
        inline_memcpy(&entry->address.v4.sin_addr, found[i].bytes, 4);
        entry->info.ai_addrlen = sizeof(struct sockaddr_in);
      } else {
        entry->address.v6 = {};
        entry->address.v6.sin6_family = AF_INET6;
        entry->address.v6.sin6_port = port;
        entry->address.v6.sin6_scope_id = found[i].scope;
        inline_memcpy(&entry->address.v6.sin6_addr, found[i].bytes, 16);
        entry->info.ai_addrlen = sizeof(struct sockaddr_in6);
      }

      if (head == nullptr)
        head = &entry->info;
      else
        tail->ai_next = &entry->info;
      tail = &entry->info;
    }
  }

  // The name the answer came back under, which only the first record carries.
  if ((flags & AI_CANONNAME) != 0 && head != nullptr) {
    const char *name = canonical_name[0] != '\0' ? canonical_name : nodename;
    if (name != nullptr) {
      const size_t length = internal::string_length(name);
      canonical = static_cast<char *>(::malloc(length + 1));
      if (canonical == nullptr) {
        LIBC_NAMESPACE::freeaddrinfo(head);
        return EAI_MEMORY;
      }
      inline_memcpy(canonical, name, length + 1);
      head->ai_canonname = canonical;
    }
  }

  *res = head;
  return 0;
}

} // namespace LIBC_NAMESPACE_DECL
