//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Linux implementation of mktemp.
///
//===----------------------------------------------------------------------===//

#include "src/stdlib/mktemp.h"
#include "hdr/errno_macros.h"
#include "hdr/fcntl_macros.h"
#include "src/__support/OSUtil/linux/stat/kernel_statx_types.h"
#include "src/__support/OSUtil/linux/syscall_wrappers/statx.h"
#include "src/__support/common.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"
#include "src/__support/macros/null_check.h"
#include "src/stdlib/linux/mktemp_util.h"

namespace LIBC_NAMESPACE_DECL {

// mktemp only says what name was free a moment ago, so whatever uses the name
// has to cope with someone else taking it first. mkstemp, which opens the
// file itself, has no such gap.
LLVM_LIBC_FUNCTION(char *, mktemp, (char *tmpl)) {
  LIBC_CRASH_ON_NULLPTR(tmpl);

  auto res = internal::mktemp_core(tmpl, [](const char *path) -> ErrorOr<int> {
    internal::kernel_statx_buf buf;
    auto found = linux_syscalls::statx(AT_FDCWD, path, AT_SYMLINK_NOFOLLOW,
                                       internal::KERNEL_STATX_TYPE_MASK, &buf);
    if (found.has_value())
      return Error(EEXIST);
    if (found.error() == ENOENT)
      return 0;
    return Error(found.error());
  });
  if (!res.has_value()) {
    libc_errno = res.error();
    // A caller which ignores errno must not go on to use the template.
    tmpl[0] = '\0';
  }
  return tmpl;
}

} // namespace LIBC_NAMESPACE_DECL
