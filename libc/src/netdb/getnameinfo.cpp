//===-- Implementation of getnameinfo -------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/netdb/getnameinfo.h"

#include "hdr/netdb_macros.h"
#include "hdr/types/socklen_t.h"
#include "hdr/types/struct_sockaddr.h"
#include "hdr/types/struct_sockaddr_in.h"
#include "hdr/types/struct_sockaddr_in6.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/arpa/inet/inet_ntop.h"
#include "src/arpa/inet/ntohs.h"
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

} // anonymous namespace

// Asks what the host and the service at an address are called, and falls
// back to the numeric form of either where there is no name for it. A caller
// which will not accept the numeric form says so with NI_NAMEREQD.
LLVM_LIBC_FUNCTION(int, getnameinfo,
                   (const struct sockaddr *__restrict addr, socklen_t addrlen,
                    char *__restrict host, socklen_t hostlen,
                    char *__restrict serv, socklen_t servlen, int flags)) {
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
    port = LIBC_NAMESPACE::ntohs(in->sin_port);
  } else if (family == AF_INET6) {
    if (addrlen < static_cast<socklen_t>(sizeof(struct sockaddr_in6)))
      return EAI_FAMILY;
    auto *in6 = reinterpret_cast<const struct sockaddr_in6 *>(addr);
    address = &in6->sin6_addr;
    port = LIBC_NAMESPACE::ntohs(in6->sin6_port);
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
      if (LIBC_NAMESPACE::inet_ntop(family, address, host, hostlen) == nullptr)
        return EAI_OVERFLOW;
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

} // namespace LIBC_NAMESPACE_DECL
