//===-- Implementation of login_tty ---------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/pty/login_tty.h"

#include "src/__support/common.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"
#include "src/sys/ioctl/ioctl.h"
#include "src/unistd/close.h"
#include "src/unistd/dup2.h"
#include "src/unistd/setsid.h"

namespace LIBC_NAMESPACE_DECL {

namespace {
// Makes the given terminal the one the session is attached to. The number is
// the kernel's, and is the same everywhere Linux runs.
constexpr unsigned long SET_CONTROLLING_TERMINAL = 0x540E; // TIOCSCTTY
} // anonymous namespace

LLVM_LIBC_FUNCTION(int, login_tty, (int fd)) {
  // A session of its own, so the terminal can be claimed.
  if (LIBC_NAMESPACE::setsid() < 0 && libc_errno != EPERM)
    return -1;

  if (LIBC_NAMESPACE::ioctl(fd, SET_CONTROLLING_TERMINAL, 0) < 0)
    return -1;

  for (int target = 0; target <= 2; ++target) {
    if (target != fd && LIBC_NAMESPACE::dup2(fd, target) < 0)
      return -1;
  }
  // The descriptor is closed once it stands in all three places.
  if (fd > 2)
    LIBC_NAMESPACE::close(fd);
  return 0;
}

} // namespace LIBC_NAMESPACE_DECL
