//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Linux implementation of ttyname_r.
///
//===----------------------------------------------------------------------===//

#include "src/unistd/ttyname_r.h"

#include "hdr/errno_macros.h"
#include "hdr/sys_ioctl_macros.h" // For ioctl numbers.
#include "hdr/types/size_t.h"
#include "src/__support/OSUtil/linux/syscall_wrappers/ioctl.h"
#include "src/__support/OSUtil/linux/syscall_wrappers/readlink.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {

namespace {

// "/proc/self/fd/" plus the widest decimal an int reaches, plus a
// terminator.
constexpr size_t PROC_PATH_SIZE = sizeof("/proc/self/fd/") + 11;

void proc_path(int fd, char (&out)[PROC_PATH_SIZE]) {
  constexpr char PREFIX[] = "/proc/self/fd/";
  size_t i = 0;
  for (; i < sizeof(PREFIX) - 1; ++i)
    out[i] = PREFIX[i];

  char digits[12];
  size_t len = 0;
  unsigned value = static_cast<unsigned>(fd);
  do {
    digits[len++] = static_cast<char>('0' + value % 10);
    value /= 10;
  } while (value != 0);
  while (len > 0)
    out[i++] = digits[--len];
  out[i] = '\0';
}

} // anonymous namespace

LLVM_LIBC_FUNCTION(int, ttyname_r, (int fd, char *buf, size_t buflen)) {
  if (buf == nullptr)
    return EINVAL;
  if (buflen == 0)
    return ERANGE;

  // Only a terminal has a terminal name. This is the check isatty makes: a
  // character device which is not a tty, /dev/null among them, fails it.
  int line_discipline = 0;
  auto tty = linux_syscalls::ioctl(fd, TIOCGETD, &line_discipline);
  if (!tty)
    return tty.error();

  char path[PROC_PATH_SIZE];
  proc_path(fd, path);

  // readlink does not terminate what it writes, and reports truncation only
  // by filling the buffer, so it is given one byte of room to spare.
  auto link = linux_syscalls::readlink(path, buf, buflen - 1);
  if (!link)
    return link.error();
  size_t written = static_cast<size_t>(link.value());
  if (written >= buflen - 1)
    return ERANGE;
  buf[written] = '\0';
  return 0;
}

} // namespace LIBC_NAMESPACE_DECL
