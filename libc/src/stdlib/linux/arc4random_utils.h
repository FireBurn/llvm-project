//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// This file contains the random bytes the arc4random functions return.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC_STDLIB_LINUX_ARC4RANDOM_UTILS_H
#define LLVM_LIBC_SRC_STDLIB_LINUX_ARC4RANDOM_UTILS_H

#include "hdr/errno_macros.h"
#include "hdr/types/size_t.h"
#include "src/__support/OSUtil/linux/syscall_wrappers/getrandom.h"
#include "src/__support/macros/attributes.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {
namespace arc4random_utils {

// The kernel generator cannot be exhausted and does not fail once it is
// seeded, which getrandom waits for. A function with no way to report an error
// has nothing to do but stop if it does.
LIBC_INLINE void fill(void *buf, size_t size) {
  auto *bytes = static_cast<unsigned char *>(buf);
  while (size > 0) {
    auto got = linux_syscalls::getrandom(bytes, size, 0);
    if (!got.has_value()) {
      if (got.error() == EINTR)
        continue;
      __builtin_trap();
    }
    bytes += got.value();
    size -= static_cast<size_t>(got.value());
  }
}

} // namespace arc4random_utils
} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC_STDLIB_LINUX_ARC4RANDOM_UTILS_H
