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

#include "hdr/sys_ioctl_macros.h"
#include "hdr/types/size_t.h"
#include "src/__support/OSUtil/linux/syscall_wrappers/ioctl.h"
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

} // namespace pty
} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC_STDLIB_LINUX_PTY_UTILS_H
