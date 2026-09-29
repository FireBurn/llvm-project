//===-- Linux implementation of isatty ------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/unistd/isatty.h"

#include "hdr/sys_ioctl_macros.h" // For ioctl numbers.
#include "src/__support/OSUtil/linux/syscall_wrappers/ioctl.h"
#include "src/__support/common.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"
#include "src/termios/linux/kernel_termios.h"

#include <asm/ioctls.h> // Safe to include without the risk of name pollution.

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(int, isatty, (int fd)) {
  // Asking for the terminal's attributes is how glibc and musl tell, and the
  // one thing the sandboxes of Firefox and Chromium answer with ENOTTY rather
  // than kill the process for.
  LIBC_NAMESPACE::kernel_termios attributes;
  auto result = linux_syscalls::ioctl(fd, TCGETS, &attributes);
  if (result.has_value())
    return 1;

  libc_errno = result.error();
  return 0;
}

} // namespace LIBC_NAMESPACE_DECL
