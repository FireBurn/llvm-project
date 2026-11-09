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
#include "hdr/errno_macros.h"
#include "hdr/func/free.h"
#include "hdr/func/malloc.h"
#include "hdr/netdb_macros.h"
#include "hdr/stdint_proxy.h"
#include "hdr/sys_socket_macros.h"
#include "hdr/types/struct_addrinfo.h"
#include "hdr/types/struct_sockaddr_storage.h"
#include "hdr/types/struct_sockaddr_un.h"
#include "hdr/types/struct_timeval.h"
#include "src/__support/common.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"
#include "src/netdb/freeaddrinfo.h"
#include "src/netdb/getaddrinfo.h"
#include "src/stdio/snprintf.h"
#include "src/string/strcspn.h"
#include "src/string/strlen.h"
#include "src/sys/socket/connect.h"
#include "src/sys/socket/recv.h"
#include "src/sys/socket/send.h"
#include "src/sys/socket/setsockopt.h"
#include "src/sys/socket/socket.h"
#include "src/unistd/close.h"

namespace LIBC_NAMESPACE_DECL {
// Android resolves names in netd, reached through dnsproxyd, using the DNS
// settings of the network chosen by id. The mark is netd's to apply. Where
// there is no dnsproxyd this is getaddrinfo.

namespace {

bool read_exact(int fd, void *buffer, size_t size) {
  return LIBC_NAMESPACE::recv(fd, buffer, size, MSG_WAITALL) ==
         static_cast<ssize_t>(size);
}

bool read_word(int fd, uint32_t &value) {
  unsigned char bytes[4];
  if (!read_exact(fd, bytes, sizeof(bytes)))
    return false;
  value = (uint32_t(bytes[0]) << 24) | (uint32_t(bytes[1]) << 16) |
          (uint32_t(bytes[2]) << 8) | uint32_t(bytes[3]);
  return true;
}

bool valid_argument(const char *value) {
  return !value || LIBC_NAMESPACE::strcspn(value, " \t\r\n^\'\"") ==
                       LIBC_NAMESPACE::strlen(value);
}

// The dnsproxyd wire protocol uses Bionic's flag values.
int proxy_flags(int flags) {
  return (flags & (AI_PASSIVE | AI_CANONNAME | AI_NUMERICHOST)) |
         ((flags & AI_NUMERICSERV) ? 0x0008 : 0) |
         ((flags & AI_ADDRCONFIG) ? 0x0400 : 0);
}

struct Entry {
  addrinfo info;
  sockaddr_storage address;
};

int read_record(int fd, Entry *entry) {
  uint32_t flags, family, type, protocol, addr_len, name_len;
  if (!read_word(fd, flags) || !read_word(fd, family) || !read_word(fd, type) ||
      !read_word(fd, protocol) || !read_word(fd, addr_len) ||
      addr_len > sizeof(entry->address) ||
      !read_exact(fd, &entry->address, addr_len) || !read_word(fd, name_len) ||
      name_len > 1024)
    return EAI_FAIL;

  entry->info.ai_flags = flags & (AI_PASSIVE | AI_CANONNAME | AI_NUMERICHOST);
  if (flags & 0x0008)
    entry->info.ai_flags |= AI_NUMERICSERV;
  if (flags & 0x0400)
    entry->info.ai_flags |= AI_ADDRCONFIG;
  entry->info.ai_family = family;
  entry->info.ai_socktype = type;
  entry->info.ai_protocol = protocol;
  entry->info.ai_addrlen = addr_len;
  entry->info.ai_addr = reinterpret_cast<sockaddr *>(&entry->address);
  if (name_len) {
    entry->info.ai_canonname = static_cast<char *>(::malloc(name_len));
    if (!entry->info.ai_canonname)
      return EAI_MEMORY;
    if (!read_exact(fd, entry->info.ai_canonname, name_len) ||
        entry->info.ai_canonname[name_len - 1] != '\0')
      return EAI_FAIL;
  }
  return 0;
}

int read_response(int fd, addrinfo **result) {
  char code[4];
  if (!read_exact(fd, code, sizeof(code)))
    return EAI_AGAIN;
  if (code[0] != '2' || code[1] != '2' || code[2] != '2' || code[3])
    return EAI_NODATA;

  addrinfo **next = result;
  for (unsigned count = 0; count < 1024; ++count) {
    uint32_t more;
    if (!read_word(fd, more))
      return EAI_FAIL;
    if (!more)
      return *result ? 0 : EAI_NODATA;
    if (more != 1)
      return EAI_FAIL;
    auto *entry = static_cast<Entry *>(::malloc(sizeof(Entry)));
    if (!entry)
      return EAI_MEMORY;
    *entry = {};
    *next = &entry->info;
    next = &entry->info.ai_next;
    if (int status = read_record(fd, entry))
      return status;
  }
  return EAI_OVERFLOW;
}

} // namespace

LLVM_LIBC_FUNCTION(int, android_getaddrinfofornet,
                   (const char *nodename, const char *servname,
                    const addrinfo *hints, unsigned netid, unsigned,
                    addrinfo **result)) {
  if (!result)
    return EAI_FAIL;
  *result = nullptr;
  if (!nodename || (hints && (hints->ai_flags & AI_NUMERICHOST)))
    return LIBC_NAMESPACE::getaddrinfo(nodename, servname, hints, result);
  if (hints && (hints->ai_flags & ~AI_MASK))
    return EAI_BADFLAGS;

  addrinfo numeric = hints ? *hints : addrinfo{};
  numeric.ai_flags |= AI_NUMERICHOST;
  if (LIBC_NAMESPACE::getaddrinfo(nodename, servname, &numeric, result) == 0)
    return 0;
  if (!valid_argument(nodename) || !valid_argument(servname))
    return EAI_NONAME;

  char command[1024];
  int length = LIBC_NAMESPACE::snprintf(
      command, sizeof(command), "getaddrinfo %s %s %d %d %d %d %u", nodename,
      servname ? servname : "^", hints ? proxy_flags(hints->ai_flags) : -1,
      hints ? hints->ai_family : -1, hints ? hints->ai_socktype : -1,
      hints ? hints->ai_protocol : -1, netid);
  if (length < 0 || static_cast<size_t>(length) >= sizeof(command))
    return EAI_NONAME;

  int fd = LIBC_NAMESPACE::socket(AF_UNIX, SOCK_STREAM | SOCK_CLOEXEC, 0);
  if (fd < 0)
    return EAI_SYSTEM;
  sockaddr_un endpoint = {AF_UNIX, "/dev/socket/dnsproxyd"};
  timeval timeout = {15, 0};
  const int previous_errno = libc_errno;
  int status = EAI_AGAIN;
  bool no_proxy = false;
  if (LIBC_NAMESPACE::setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &timeout,
                                 sizeof(timeout)) == 0 &&
      LIBC_NAMESPACE::setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, &timeout,
                                 sizeof(timeout)) == 0) {
    if (LIBC_NAMESPACE::connect(fd, reinterpret_cast<sockaddr *>(&endpoint),
                                sizeof(endpoint)) != 0)
      no_proxy = libc_errno == ENOENT;
    else if (LIBC_NAMESPACE::send(fd, command, length + 1, MSG_NOSIGNAL) ==
             length + 1)
      status = read_response(fd, result);
  }
  LIBC_NAMESPACE::close(fd);
  if (no_proxy) {
    libc_errno = previous_errno;
    return LIBC_NAMESPACE::getaddrinfo(nodename, servname, hints, result);
  }
  if (status) {
    LIBC_NAMESPACE::freeaddrinfo(*result);
    *result = nullptr;
  }
  return status;
}

} // namespace LIBC_NAMESPACE_DECL
