//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// The bit the pseudoterminal calls share
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC_STDLIB_LINUX_PTY_UTILS_H
#define LLVM_LIBC_SRC_STDLIB_LINUX_PTY_UTILS_H

#include "hdr/errno_macros.h"
#include "hdr/fcntl_macros.h"
#include "hdr/sys_ioctl_macros.h"
#include "hdr/types/size_t.h"
#include "src/__support/OSUtil/linux/syscall_wrappers/ioctl.h"
#include "src/__support/OSUtil/linux/syscall_wrappers/openat.h"
#include "src/__support/integer_to_string.h"
#include "src/__support/macros/attributes.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {
namespace pty {

// Where the multiplexer lives, and the shape of the names it hands out.
LIBC_INLINE_VAR constexpr const char *PTMX_PATH = "/dev/ptmx";
LIBC_INLINE_VAR constexpr const char *PTS_PREFIX = "/dev/pts/";
LIBC_INLINE_VAR constexpr size_t PTS_PREFIX_LEN = 9;

// Asks the kernel which numbered terminal a master descriptor is for.
// Returns an errno value, or zero.
LIBC_INLINE int number(int fd, unsigned int *out) {
  ErrorOr<int> ret = linux_syscalls::ioctl(fd, TIOCGPTN, out);
  return ret ? 0 : ret.error();
}

// Opens the multiplexer, which hands back a master with a terminal of its
// own behind it.
LIBC_INLINE ErrorOr<int> open_master(int flags) {
  return linux_syscalls::openat(AT_FDCWD, PTMX_PATH, flags, 0);
}

// Lets the terminal behind the master be opened. Returns an errno value, or
// zero.
LIBC_INLINE int unlock(int fd) {
  int unlock = 0;
  ErrorOr<int> ret = linux_syscalls::ioctl(fd, TIOCSPTLCK, &unlock);
  return ret ? 0 : ret.error();
}

// Writes the path of the terminal behind a master. Returns an errno value, or
// zero.
LIBC_INLINE int name(int fd, char *buf, size_t buflen) {
  if (buf == nullptr)
    return EINVAL;

  unsigned int num = 0;
  if (int err = number(fd, &num); err != 0)
    return err;

  const IntegerToString<unsigned int> digits(num);
  auto view = digits.view();
  if (PTS_PREFIX_LEN + view.size() + 1 > buflen)
    return ERANGE;

  for (size_t i = 0; i < PTS_PREFIX_LEN; ++i)
    buf[i] = PTS_PREFIX[i];
  for (size_t i = 0; i < view.size(); ++i)
    buf[PTS_PREFIX_LEN + i] = view[i];
  buf[PTS_PREFIX_LEN + view.size()] = '\0';
  return 0;
}

} // namespace pty
} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC_STDLIB_LINUX_PTY_UTILS_H
