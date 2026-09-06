//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation of openpty
///
//===----------------------------------------------------------------------===//

#include "src/pty/openpty.h"

#include "hdr/errno_macros.h"
#include "hdr/fcntl_macros.h"
#include "hdr/termios_macros.h"
#include "src/__support/OSUtil/linux/syscall_wrappers/close.h"
#include "src/__support/OSUtil/linux/syscall_wrappers/ioctl.h"
#include "src/__support/OSUtil/linux/syscall_wrappers/openat.h"
#include "src/__support/common.h"
#include "src/__support/error_or.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"
#include "src/stdlib/linux/pty_utils.h"
#include "src/string/string_utils.h"
#include "src/termios/tcsetattr.h"

namespace LIBC_NAMESPACE_DECL {

namespace {
// Sets the size a terminal reports. The number is the kernel's, and is the
// same everywhere Linux runs.
constexpr unsigned long SET_WINDOW_SIZE = 0x5414; // TIOCSWINSZ
// The longest a pseudo terminal's path runs to.
constexpr size_t MAX_PTY_NAME = 64;
} // anonymous namespace

namespace internal {

ErrorOr<int> openpty_impl(int *primary, int *secondary, char *name,
                          const struct termios *settings,
                          const struct winsize *size) {
  if (primary == nullptr || secondary == nullptr)
    return Error(EINVAL);

  ErrorOr<int> first = pty::open_master(O_RDWR | O_NOCTTY);
  if (!first)
    return first;

  char path[MAX_PTY_NAME];
  unsigned int number = 0;
  int err = pty::number(*first, &number);
  if (err == 0)
    err = pty::unlock(*first);
  if (err == 0)
    err = pty::name(*first, path, sizeof(path));
  if (err != 0) {
    linux_syscalls::close(*first);
    return Error(err);
  }

  ErrorOr<int> second =
      linux_syscalls::openat(AT_FDCWD, path, O_RDWR | O_NOCTTY, 0);
  if (!second) {
    linux_syscalls::close(*first);
    return second;
  }

  // What the caller asked the terminal to be like, where it asked at all.
  if (settings != nullptr)
    internal::tcsetattr_impl(*second, TCSAFLUSH, settings);
  if (size != nullptr)
    linux_syscalls::ioctl(*second, SET_WINDOW_SIZE, size);

  if (name != nullptr)
    internal::strlcpy(name, path, MAX_PTY_NAME);

  *primary = *first;
  *secondary = *second;
  return 0;
}

} // namespace internal

LLVM_LIBC_FUNCTION(int, openpty,
                   (int *primary, int *secondary, char *name,
                    const struct termios *settings,
                    const struct winsize *size)) {
  if (auto ret =
          internal::openpty_impl(primary, secondary, name, settings, size);
      !ret) {
    libc_errno = ret.error();
    return -1;
  }
  return 0;
}

} // namespace LIBC_NAMESPACE_DECL
