//===-- Implementation of mktemp ------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
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
#include "src/stdlib/temp_template.h"

namespace LIBC_NAMESPACE_DECL {

// mktemp only says what name was free a moment ago, so whatever uses the name
// has to cope with someone else taking it first. mkstemp, which opens the
// file itself, has no such gap and is what a caller should reach for.
LLVM_LIBC_FUNCTION(char *, mktemp, (char *tmpl)) {
  LIBC_CRASH_ON_NULLPTR(tmpl);

  auto suffix = temp_template::suffix_of(tmpl);
  if (!suffix.has_value()) {
    libc_errno = suffix.error();
    // A failure leaves an empty string behind, so a caller which ignored the
    // errno cannot go on to use the template as a name.
    tmpl[0] = '\0';
    return tmpl;
  }

  for (int attempt = 0; attempt < temp_template::MAX_ATTEMPTS; ++attempt) {
    if (auto ok = temp_template::randomize(suffix.value()); !ok.has_value()) {
      libc_errno = ok.error();
      tmpl[0] = '\0';
      return tmpl;
    }
    internal::kernel_statx_buf buf;
    auto exists = linux_syscalls::statx(AT_FDCWD, tmpl, 0,
                                        internal::KERNEL_STATX_TYPE_MASK, &buf);
    if (!exists)
      return tmpl; // Nothing of that name, which is what was wanted.
  }
  libc_errno = EEXIST;
  tmpl[0] = '\0';
  return tmpl;
}

} // namespace LIBC_NAMESPACE_DECL
