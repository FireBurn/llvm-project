//===-- The bit the pseudoterminal calls share ------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC_STDLIB_LINUX_PTY_UTILS_H
#define LLVM_LIBC_SRC_STDLIB_LINUX_PTY_UTILS_H

#include "hdr/sys_ioctl_macros.h"
#include "hdr/types/size_t.h"
#include "src/__support/OSUtil/syscall.h"
#include "src/__support/macros/attributes.h"
#include "src/__support/macros/config.h"
#include <sys/syscall.h>

namespace LIBC_NAMESPACE_DECL {
namespace pty {

// Where the multiplexer lives, and the shape of the names it hands out.
LIBC_INLINE_VAR constexpr const char *PTMX_PATH = "/dev/ptmx";
LIBC_INLINE_VAR constexpr const char *PTS_PREFIX = "/dev/pts/";
LIBC_INLINE_VAR constexpr size_t PTS_PREFIX_LEN = 9;

// Asks the kernel which numbered terminal a master descriptor is for.
// Returns an errno value, or zero.
LIBC_INLINE int number(int fd, unsigned int *out) {
  int ret = LIBC_NAMESPACE::syscall_impl<int>(SYS_ioctl, fd, TIOCGPTN, out);
  return ret < 0 ? -ret : 0;
}

} // namespace pty
} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC_STDLIB_LINUX_PTY_UTILS_H
