//===-- The state a syslog connection keeps -------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/syslog/syslog_state.h"

#include "hdr/fcntl_macros.h"
#include "src/__support/OSUtil/syscall.h"
#include "src/__support/macros/config.h"
#include "src/__support/threads/mutex.h"

#include <sys/socket.h>
#include <sys/syscall.h>
#include <sys/un.h>

namespace LIBC_NAMESPACE_DECL {
namespace syslog_internal {

State log_state;
Mutex log_mutex(false, false, false, false);

namespace {

int open_socket(int type) {
  int fd = LIBC_NAMESPACE::syscall_impl<int>(SYS_socket, AF_UNIX,
                                             type | SOCK_CLOEXEC, 0);
  if (fd < 0)
    return -1;

  struct sockaddr_un addr;
  addr.sun_family = AF_UNIX;
  const char *p = LOG_PATH;
  size_t i = 0;
  for (; p[i] != '\0' && i + 1 < sizeof(addr.sun_path); ++i)
    addr.sun_path[i] = p[i];
  addr.sun_path[i] = '\0';

  int ret = LIBC_NAMESPACE::syscall_impl<int>(
      SYS_connect, fd, reinterpret_cast<struct sockaddr *>(&addr),
      static_cast<socklen_t>(sizeof(addr)));
  if (ret < 0) {
    LIBC_NAMESPACE::syscall_impl<int>(SYS_close, fd);
    return -1;
  }
  return fd;
}

} // namespace

bool connect_unlocked() {
  State &s = log_state;
  if (s.fd >= 0)
    return true;

  // The daemon usually listens on a datagram socket, but a stream one is
  // allowed and some do use it, so fall back to that.
  int fd = open_socket(SOCK_DGRAM);
  if (fd >= 0) {
    s.fd = fd;
    s.stream = false;
    return true;
  }
  fd = open_socket(SOCK_STREAM);
  if (fd >= 0) {
    s.fd = fd;
    s.stream = true;
    return true;
  }
  return false;
}

void disconnect_unlocked() {
  State &s = log_state;
  if (s.fd >= 0)
    LIBC_NAMESPACE::syscall_impl<int>(SYS_close, s.fd);
  s.fd = -1;
  s.stream = false;
}

} // namespace syslog_internal
} // namespace LIBC_NAMESPACE_DECL
