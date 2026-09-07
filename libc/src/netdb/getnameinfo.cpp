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

#include "hdr/net_if_macros.h"
#include "hdr/netdb_macros.h"
#include "hdr/types/socklen_t.h"
#include "hdr/types/struct_sockaddr.h"
#include "hdr/types/struct_sockaddr_in.h"
#include "hdr/types/struct_sockaddr_in6.h"
#include "src/__support/common.h"
#include "src/__support/endian_internal.h"
#include "src/__support/macros/config.h"
#include "src/arpa/inet/inet_ntop.h"
#include "src/net/if/if_indextoname.h"
#include "src/netdb/resolv/lookup.h"
#include "src/string/memory_utils/inline_memcpy.h"
#include "src/string/string_utils.h"

#include <sys/socket.h>

namespace LIBC_NAMESPACE_DECL {

namespace {

// Writes |value| in decimal. Returns how many characters it took, or 0 if
// there was not room for all of them and the terminator.
size_t write_number(unsigned value, char *out, size_t outlen) {
  char digits[8];
  size_t len = 0;
  do {
    digits[len++] = static_cast<char>('0' + value % 10);
    value /= 10;
  } while (value != 0);
  if (len + 1 > outlen)
    return 0;
  for (size_t i = 0; i < len; ++i)
    out[i] = digits[len - 1 - i];
  out[len] = '\0';
  return len;
}

// An address which only means something on one interface says which, after a
// percent sign, as the numeric form of it is written: link local addresses,
// and multicast ones of interface or link scope.
bool needs_scope(const unsigned char *bytes) {
  if (bytes[0] == 0xfe && (bytes[1] & 0xc0) == 0x80)
    return true;
  return bytes[0] == 0xff && ((bytes[1] & 0x0f) == 1 || (bytes[1] & 0x0f) == 2);
}

// Adds the scope to the text in |host|, naming the interface unless a number
// is asked for or the interface has no name. Returns false if it did not fit.
bool append_scope(char *host, size_t hostlen, uint32_t scope, int flags) {
  size_t used = 0;
  while (host[used] != '\0')
    ++used;
  if (used + 2 > hostlen)
    return false;
  host[used++] = '%';
  char name[IF_NAMESIZE];
  if (!(flags & NI_NUMERICSCOPE) &&
      internal::if_indextoname_impl(scope, name) != nullptr) {
    size_t length = 0;
    while (name[length] != '\0')
      ++length;
    if (used + length + 1 > hostlen)
      return false;
    inline_memcpy(host + used, name, length + 1);
    return true;
  }
  return write_number(scope, host + used, hostlen - used) != 0;
}

} // anonymous namespace

// Asks what the host and the service at an address are called, and falls
// back to the numeric form of either where there is no name for it. A caller
// which will not accept the numeric form says so with NI_NAMEREQD.
namespace internal {

int getnameinfo_impl(const struct sockaddr *__restrict addr, socklen_t addrlen,
                     char *__restrict host, socklen_t hostlen,
                     char *__restrict serv, socklen_t servlen, int flags) {
  if (addr == nullptr)
    return EAI_FAMILY;
  if ((host == nullptr || hostlen == 0) && (serv == nullptr || servlen == 0))
    return EAI_NONAME;

  int family = addr->sa_family;
  const void *address = nullptr;
  unsigned port = 0;

  if (family == AF_INET) {
    if (addrlen < static_cast<socklen_t>(sizeof(struct sockaddr_in)))
      return EAI_FAMILY;
    auto *in = reinterpret_cast<const struct sockaddr_in *>(addr);
    address = &in->sin_addr;
    port = LIBC_NAMESPACE::Endian::from_big_endian(in->sin_port);
  } else if (family == AF_INET6) {
    if (addrlen < static_cast<socklen_t>(sizeof(struct sockaddr_in6)))
      return EAI_FAMILY;
    auto *in6 = reinterpret_cast<const struct sockaddr_in6 *>(addr);
    address = &in6->sin6_addr;
    port = LIBC_NAMESPACE::Endian::from_big_endian(in6->sin6_port);
  } else {
    return EAI_FAMILY;
  }

  if (host != nullptr && hostlen > 0) {
    bool named = false;
    if (!(flags & NI_NUMERICHOST)) {
      char name[resolv::MAX_NAME + 1];
      if (resolv::lookup_address(
              reinterpret_cast<const unsigned char *>(address), family, name,
              sizeof(name))) {
        size_t written = 0;
        while (name[written] != '\0')
          ++written;
        if (written >= hostlen)
          return EAI_OVERFLOW;
        inline_memcpy(host, name, written + 1);
        named = true;
      }
    }
    if (!named) {
      // Nothing names it, so the numeric form is all there is to report, and
      // a caller which insisted on a name is told there is none.
      if (flags & NI_NAMEREQD)
        return EAI_NONAME;
      if (internal::inet_ntop_impl(family, address, host, hostlen) == nullptr)
        return EAI_OVERFLOW;
      if (family == AF_INET6) {
        auto *in6 = reinterpret_cast<const struct sockaddr_in6 *>(addr);
        if (in6->sin6_scope_id != 0 &&
            needs_scope(reinterpret_cast<const unsigned char *>(address)) &&
            !append_scope(host, hostlen, in6->sin6_scope_id, flags))
          return EAI_OVERFLOW;
      }
    }
  }

  if (serv != nullptr && servlen > 0) {
    // /etc/services is not read, so the port is reported as a number, which
    // is what NI_NUMERICSERV asks for in any case.
    if (write_number(port, serv, servlen) == 0)
      return EAI_OVERFLOW;
  }

  return 0;
}

} // namespace internal

LLVM_LIBC_FUNCTION(int, getnameinfo,
                   (const struct sockaddr *__restrict addr, socklen_t addrlen,
                    char *__restrict host, socklen_t hostlen,
                    char *__restrict serv, socklen_t servlen, int flags)) {
  return internal::getnameinfo_impl(addr, addrlen, host, hostlen, serv, servlen,
                                    flags);
}

} // namespace LIBC_NAMESPACE_DECL
