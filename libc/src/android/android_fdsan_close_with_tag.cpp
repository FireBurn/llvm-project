//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation of android_fdsan_close_with_tag.
///
//===----------------------------------------------------------------------===//

#include "src/android/android_fdsan_close_with_tag.h"
#include "src/__support/OSUtil/linux/syscall_wrappers/close.h"
#include "src/__support/common.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"
#include "src/__support/threads/cancel.h"

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(int, android_fdsan_close_with_tag, (int fd, uint64_t tag)) {
  // With no owner recorded, any tag matches.
  (void)tag;
  auto result =
      internal::cancellable([&] { return linux_syscalls::close(fd); });
  if (!result.has_value()) {
    libc_errno = result.error();
    return -1;
  }
  return result.value();
}

} // namespace LIBC_NAMESPACE_DECL
