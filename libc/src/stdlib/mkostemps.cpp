//===-- Implementation of mkostemps ---------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// mkstemps with a say in how the file is opened.
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
#include "src/stdlib/temp_template.h"

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(int, mkostemps, (char *tmpl, int suffix_length, int flags)) {
  LIBC_CRASH_ON_NULLPTR(tmpl);

  // The file has to be made, has to be new and has to be readable and
  // writable, so those three are not the caller's to choose. What is left
  // is what the caller may add, such as O_CLOEXEC.
  int open_flags = (flags) & ~(O_ACCMODE | O_CREAT | O_EXCL);

  auto suffix = temp_template::suffix_of(tmpl, suffix_length);
  if (!suffix.has_value()) {
    libc_errno = suffix.error();
    return -1;
  }

  for (int attempt = 0; attempt < temp_template::MAX_ATTEMPTS; ++attempt) {
    if (auto ok = temp_template::randomize(suffix.value()); !ok.has_value()) {
      libc_errno = ok.error();
      return -1;
    }
    auto fd = linux_syscalls::open(tmpl, O_RDWR | O_CREAT | O_EXCL | open_flags,
                                   0600);
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
