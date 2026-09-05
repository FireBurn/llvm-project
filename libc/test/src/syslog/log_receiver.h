//===-- A socket for the syslog tests to log to -----------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_TEST_SRC_SYSLOG_LOG_RECEIVER_H
#define LLVM_LIBC_TEST_SRC_SYSLOG_LOG_RECEIVER_H

#include "hdr/sys_socket_macros.h"
#include "hdr/types/size_t.h"
#include "hdr/types/ssize_t.h"
#include "hdr/types/struct_sockaddr_un.h"
#include "src/__support/CPP/mutex.h"
#include "src/__support/CPP/string_view.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"
#include "src/sys/socket/bind.h"
#include "src/sys/socket/recv.h"
#include "src/sys/socket/socket.h"
#include "src/syslog/closelog.h"
#include "src/syslog/syslog_state.h"
#include "src/unistd/close.h"
#include "src/unistd/unlink.h"
#include "test/src/sys/socket/linux/socket_test_support.h"

namespace LIBC_NAMESPACE_DECL {
namespace testing {

// Points the log at a datagram socket the test owns, so that the test can read
// back what it logged and nothing it logs reaches the system's log.
class LogReceiver {
  TestDirectoryScope dir_scope;
  const char *path;
  int fd = -1;

  static void use_path(const char *p) {
    cpp::lock_guard guard(syslog_internal::log_mutex);
    syslog_internal::log_state.path = p;
  }

public:
  // Without |listen| nothing is bound at the path, which is how a system with
  // no log daemon looks.
  explicit LogReceiver(const char *p, bool listen = true) : path(p) {
    LIBC_NAMESPACE::unlink(path);
    if (listen) {
      struct sockaddr_un addr;
      if (!make_sockaddr_un(path, addr))
        __builtin_trap();
      fd = LIBC_NAMESPACE::socket(AF_UNIX, SOCK_DGRAM, 0);
      if (fd < 0 ||
          LIBC_NAMESPACE::bind(fd, reinterpret_cast<struct sockaddr *>(&addr),
                               sizeof(addr)) != 0)
        __builtin_trap();
    }
    // A connection made earlier still goes wherever the log pointed then.
    LIBC_NAMESPACE::closelog();
    use_path(path);
    libc_errno = 0;
  }

  ~LogReceiver() {
    LIBC_NAMESPACE::closelog();
    use_path(syslog_internal::LOG_PATH);
    if (fd >= 0)
      LIBC_NAMESPACE::close(fd);
    LIBC_NAMESPACE::unlink(path);
    libc_errno = 0;
  }

  LogReceiver(const LogReceiver &) = delete;
  LogReceiver &operator=(const LogReceiver &) = delete;

  // The next record, or an empty view if none is waiting.
  cpp::string_view next(char *buf, size_t cap) {
    ssize_t n = LIBC_NAMESPACE::recv(fd, buf, cap, MSG_DONTWAIT);
    if (n <= 0) {
      libc_errno = 0;
      return cpp::string_view();
    }
    return cpp::string_view(buf, static_cast<size_t>(n));
  }
};

} // namespace testing
} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_TEST_SRC_SYSLOG_LOG_RECEIVER_H
