//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation of mkstemp, a POSIX function that creates a unique temporary
/// file from a template string ending in at least six 'X' characters.
///
/// Replaces the trailing X's with random characters from the POSIX portable
/// filename character set, opens the file exclusively, and returns an open
/// file descriptor, retrying automatically on name collision. See:
/// https://pubs.opengroup.org/onlinepubs/9799919799/functions/mkdtemp.html
///
//===----------------------------------------------------------------------===//

#include "src/stdlib/mkstemp.h"
#include "hdr/errno_macros.h"
#include "hdr/fcntl_macros.h"
#include "src/__support/OSUtil/linux/syscall_wrappers/open.h"
#include "src/__support/common.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"
#include "src/__support/macros/null_check.h"
#include "src/stdlib/temp_template.h"

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(int, mkstemp, (char *tmpl)) {
  LIBC_CRASH_ON_NULLPTR(tmpl);

  auto suffix = temp_template::suffix_of(tmpl);
  if (!suffix.has_value()) {
    libc_errno = suffix.error();
    return -1;
  }

  for (int attempt = 0; attempt < temp_template::MAX_ATTEMPTS; ++attempt) {
    if (auto ok = temp_template::randomize(suffix.value()); !ok.has_value()) {
      libc_errno = ok.error();
      return -1;
    }
    auto fd = linux_syscalls::open(tmpl, O_RDWR | O_CREAT | O_EXCL, 0600);
    if (fd.has_value())
      return fd.value();
    if (fd.error() != EEXIST) {
      libc_errno = fd.error();
      return -1;
    }
  }
  libc_errno = EEXIST;
  return -1;
}

} // namespace LIBC_NAMESPACE_DECL
