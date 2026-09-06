//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation of login_tty
///
//===----------------------------------------------------------------------===//

#include "src/pty/login_tty.h"

#include "hdr/errno_macros.h"
#include "src/__support/OSUtil/linux/syscall_wrappers/close.h"
#include "src/__support/OSUtil/linux/syscall_wrappers/dup2.h"
#include "src/__support/OSUtil/linux/syscall_wrappers/ioctl.h"
#include "src/__support/OSUtil/linux/syscall_wrappers/setsid.h"
#include "src/__support/common.h"
#include "src/__support/error_or.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {

namespace {
// Makes the given terminal the one the session is attached to. The number is
// the kernel's, and is the same everywhere Linux runs.
constexpr unsigned long SET_CONTROLLING_TERMINAL = 0x540E; // TIOCSCTTY
} // anonymous namespace

namespace internal {

ErrorOr<int> login_tty_impl(int fd) {
  // A session of its own, so the terminal can be claimed.
  if (auto sid = linux_syscalls::setsid(); !sid && sid.error() != EPERM)
    return Error(sid.error());

  if (auto ret = linux_syscalls::ioctl(fd, SET_CONTROLLING_TERMINAL, 0); !ret)
    return Error(ret.error());

  for (int target = 0; target <= 2; ++target) {
    if (target == fd)
      continue;
    if (auto ret = linux_syscalls::dup2(fd, target); !ret)
      return Error(ret.error());
  }
  // The descriptor is closed once it stands in all three places.
  if (fd > 2)
    linux_syscalls::close(fd);
  return 0;
}

} // namespace internal

LLVM_LIBC_FUNCTION(int, login_tty, (int fd)) {
  if (auto ret = internal::login_tty_impl(fd); !ret) {
    libc_errno = ret.error();
    return -1;
  }
  return 0;
}

} // namespace LIBC_NAMESPACE_DECL
