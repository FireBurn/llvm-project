//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// mkstemp for a template that ends in a suffix to keep.
///
//===----------------------------------------------------------------------===//

#include "src/stdlib/mkstemps.h"
#include "hdr/errno_macros.h"
#include "hdr/fcntl_macros.h"
#include "src/__support/OSUtil/linux/syscall_wrappers/open.h"
#include "src/__support/common.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"
#include "src/__support/macros/null_check.h"
#include "src/stdlib/linux/mktemp_util.h"

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(int, mkstemps, (char *tmpl, int suffix_len)) {
  LIBC_CRASH_ON_NULLPTR(tmpl);

  if (suffix_len < 0) {
    libc_errno = EINVAL;
    return -1;
  }

  auto res = internal::mktemp_core(
      tmpl,
      [](const char *path) {
        return linux_syscalls::open(path, O_RDWR | O_CREAT | O_EXCL, 0600);
      },
      static_cast<size_t>(suffix_len));
  if (!res.has_value()) {
    libc_errno = res.error();
    return -1;
  }
  return res.value();
}

} // namespace LIBC_NAMESPACE_DECL
