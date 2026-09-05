//===-- The state a syslog connection keeps ---------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC_SYSLOG_SYSLOG_STATE_H
#define LLVM_LIBC_SRC_SYSLOG_SYSLOG_STATE_H

#include "hdr/syslog_macros.h"
#include "src/__support/macros/attributes.h"
#include "src/__support/macros/config.h"
#include "src/__support/threads/mutex.h"

namespace LIBC_NAMESPACE_DECL {
namespace syslog_internal {

// Where the log daemon listens. This is where it has always been, and no
// program passes it in.
LIBC_INLINE_VAR constexpr const char *LOG_PATH = "/dev/log";

// The one connection the whole program shares, and the settings openlog
// left behind. Everything here is reached under the lock.
struct State {
  int fd = -1;
  // Whether the socket which took the connection is a stream one. A datagram
  // socket is tried first; a stream one needs the record terminated.
  bool stream = false;
  const char *ident = nullptr;
  int option = 0;
  int facility = LOG_USER;
  int mask = 0xff;
};

extern State log_state;
extern Mutex log_mutex;

// Makes the connection if it is not already up. Returns false if it could
// not be made.
bool connect_unlocked();

// Drops the connection.
void disconnect_unlocked();

} // namespace syslog_internal
} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC_SYSLOG_SYSLOG_STATE_H
