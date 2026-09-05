//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// mkostemp for a template that ends in a suffix to keep.
///
//===----------------------------------------------------------------------===//

#include "src/stdlib/mkostemps.h"
#include "hdr/errno_macros.h"
#include "hdr/fcntl_macros.h"
#include "src/__support/OSUtil/linux/syscall_wrappers/open.h"
#include "src/__support/common.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"
#include "src/__support/macros/null_check.h"
#include "src/stdlib/linux/mktemp_util.h"

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(int, mkostemps, (char *tmpl, int suffix_len, int flags)) {
  LIBC_CRASH_ON_NULLPTR(tmpl);

  // POSIX.1-2024 restricts flags to these, as for mkostemp.
  constexpr int ALLOWED_FLAGS = O_APPEND | O_CLOEXEC | O_DSYNC | O_SYNC;
  if ((flags & ~ALLOWED_FLAGS) != 0) {
    libc_errno = EINVAL;
    return -1;
  }
  if (suffix_len < 0) {
    libc_errno = EINVAL;
    return -1;
  }

  auto res = internal::mktemp_core(
      tmpl,
      [flags](const char *path) {
        return linux_syscalls::open(path, O_RDWR | O_CREAT | O_EXCL | flags, 0600);
      },
      static_cast<size_t>(suffix_len));
  if (!res.has_value()) {
    libc_errno = res.error();
    return -1;
  }
  return res.value();
}

} // namespace LIBC_NAMESPACE_DECL
