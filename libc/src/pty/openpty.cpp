//===-- Implementation of openpty -----------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/pty/openpty.h"

#include "hdr/fcntl_macros.h"
#include "src/__support/common.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"
#include "src/fcntl/open.h"
#include "src/stdlib/grantpt.h"
#include "src/stdlib/posix_openpt.h"
#include "src/stdlib/ptsname_r.h"
#include "src/stdlib/unlockpt.h"
#include "src/string/string_utils.h"
#include "src/sys/ioctl/ioctl.h"
#include "src/termios/tcsetattr.h"
#include "src/unistd/close.h"

namespace LIBC_NAMESPACE_DECL {

namespace {
// Sets the size a terminal reports. The number is the kernel's, and is the
// same everywhere Linux runs.
constexpr unsigned long SET_WINDOW_SIZE = 0x5414; // TIOCSWINSZ
// The longest a pseudo terminal's path runs to.
constexpr size_t MAX_PTY_NAME = 64;
} // anonymous namespace

LLVM_LIBC_FUNCTION(int, openpty,
                   (int *primary, int *secondary, char *name,
                    const struct termios *settings,
                    const struct winsize *size)) {
  if (primary == nullptr || secondary == nullptr) {
    libc_errno = EINVAL;
    return -1;
  }

  int first = LIBC_NAMESPACE::posix_openpt(O_RDWR | O_NOCTTY);
  if (first < 0)
    return -1;

  char path[MAX_PTY_NAME];
  if (LIBC_NAMESPACE::grantpt(first) != 0 ||
      LIBC_NAMESPACE::unlockpt(first) != 0 ||
      LIBC_NAMESPACE::ptsname_r(first, path, sizeof(path)) != 0) {
    const int saved = libc_errno;
    LIBC_NAMESPACE::close(first);
    libc_errno = saved;
    return -1;
  }

  int second = LIBC_NAMESPACE::open(path, O_RDWR | O_NOCTTY, 0);
  if (second < 0) {
    const int saved = libc_errno;
    LIBC_NAMESPACE::close(first);
    libc_errno = saved;
    return -1;
  }

  // What the caller asked the terminal to be like, where it asked at all.
  if (settings != nullptr)
    LIBC_NAMESPACE::tcsetattr(second, TCSAFLUSH, settings);
  if (size != nullptr)
    LIBC_NAMESPACE::ioctl(second, SET_WINDOW_SIZE, size);

  if (name != nullptr)
    internal::strlcpy(name, path, MAX_PTY_NAME);

  *primary = first;
  *secondary = second;
  return 0;
}

} // namespace LIBC_NAMESPACE_DECL
