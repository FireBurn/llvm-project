//===-- Implementation of mkdtemp -----------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/stdlib/mkdtemp.h"

#include "hdr/errno_macros.h"
#include "src/__support/OSUtil/linux/syscall_wrappers/mkdir.h"
#include "src/__support/common.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"
#include "src/__support/macros/null_check.h"
#include "src/stdlib/temp_template.h"

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(char *, mkdtemp, (char *tmpl)) {
  LIBC_CRASH_ON_NULLPTR(tmpl);

  auto suffix = temp_template::suffix_of(tmpl);
  if (!suffix.has_value()) {
    libc_errno = suffix.error();
    return nullptr;
  }

  for (int attempt = 0; attempt < temp_template::MAX_ATTEMPTS; ++attempt) {
    if (auto ok = temp_template::randomize(suffix.value()); !ok.has_value()) {
      libc_errno = ok.error();
      return nullptr;
    }
    // The directory is the caller's alone, which is the whole point of it.
    auto made = linux_syscalls::mkdir(tmpl, 0700);
    if (made.has_value())
      return tmpl;
    if (made.error() != EEXIST) {
      libc_errno = made.error();
      return nullptr;
    }
  }
  libc_errno = EEXIST;
  return nullptr;
}

} // namespace LIBC_NAMESPACE_DECL
