//===-- One exchange with a name server -----------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/netdb/resolv/exchange.h"

#include "hdr/sys_socket_macros.h"
#include "hdr/types/struct_pollfd.h"
#include "hdr/types/struct_sockaddr_in.h"
#include "hdr/types/struct_sockaddr_in6.h"
#include "include/llvm-libc-macros/linux/poll-macros.h"
#include "src/__support/macros/config.h"
#include "src/poll/poll.h"
#include "src/string/memory_utils/inline_memcpy.h"
#include "src/sys/socket/recvfrom.h"
#include "src/sys/socket/sendto.h"
#include "src/sys/socket/socket.h"
#include "src/unistd/close.h"

namespace LIBC_NAMESPACE_DECL {
namespace resolv {

namespace {

// The identifier the reply must carry to be an answer to this question.
bool answers(const unsigned char *message, size_t length,
             const unsigned char *reply, size_t reply_length) {
  return length >= 2 && reply_length >= 2 && reply[0] == message[0] &&
         reply[1] == message[1];
}

} // anonymous namespace

int exchange(const ResolvConf &conf, const unsigned char *message,
             size_t length, unsigned char *answer, size_t capacity) {
  if (message == nullptr || answer == nullptr || length == 0)
    return -1;

  for (size_t attempt = 0; attempt < conf.attempts; ++attempt) {
    for (size_t i = 0; i < conf.server_count; ++i) {
      const Nameserver &server = conf.servers[i];
      int fd =
          LIBC_NAMESPACE::socket(server.family, SOCK_DGRAM | SOCK_CLOEXEC, 0);
      if (fd < 0)
        continue;

      ssize_t sent;
      if (server.family == AF_INET) {
        struct sockaddr_in to = {};
        to.sin_family = AF_INET;
        to.sin_port = server.port;
        inline_memcpy(&to.sin_addr, server.bytes, 4);
        sent = LIBC_NAMESPACE::sendto(fd, message, length, 0,
                                      reinterpret_cast<struct sockaddr *>(&to),
                                      sizeof(to));
      } else {
        struct sockaddr_in6 to = {};
        to.sin6_family = AF_INET6;
        to.sin6_port = server.port;
        to.sin6_scope_id = server.scope;
        inline_memcpy(&to.sin6_addr, server.bytes, 16);
        sent = LIBC_NAMESPACE::sendto(fd, message, length, 0,
                                      reinterpret_cast<struct sockaddr *>(&to),
                                      sizeof(to));
      }
      if (sent < 0) {
        LIBC_NAMESPACE::close(fd);
        continue;
      }

      struct pollfd waiting = {fd, POLLIN, 0};
      const int ready = LIBC_NAMESPACE::poll(
          &waiting, 1, static_cast<int>(conf.timeout_seconds) * 1000);
      if (ready <= 0) {
        LIBC_NAMESPACE::close(fd);
        continue;
      }

      const ssize_t taken =
          LIBC_NAMESPACE::recvfrom(fd, answer, capacity, 0, nullptr, nullptr);
      LIBC_NAMESPACE::close(fd);
      if (taken <= 0)
        continue;
      // A reply carrying somebody else's identifier answers somebody else's
      // question, so it is waited past rather than handed back.
      if (!answers(message, length, answer, static_cast<size_t>(taken)))
        continue;
      return static_cast<int>(taken);
    }
  }
  return -1;
}

} // namespace resolv
} // namespace LIBC_NAMESPACE_DECL
