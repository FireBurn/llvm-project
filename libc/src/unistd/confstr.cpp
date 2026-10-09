//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation of confstr
///
//===----------------------------------------------------------------------===//

#include "src/unistd/confstr.h"

#include "hdr/errno_macros.h"
#include "hdr/types/size_t.h"
#include "hdr/unistd_macros.h"
#include "src/__support/common.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(size_t, confstr, (int name, char *buf, size_t len)) {
  // _CS_PATH is the directories in which the standard utilities are
  // guaranteed to be found, which is what a caller asks for when it must not
  // trust the inherited PATH. The two glibc names give the library's own name,
  // for programs that report it.
  const char *value = nullptr;
  size_t needed = 0;
  switch (name) {
  case _CS_PATH:
    value = "/bin:/usr/bin";
    needed = sizeof("/bin:/usr/bin");
    break;
  case _CS_GNU_LIBC_VERSION:
  case _CS_GNU_LIBPTHREAD_VERSION:
    value = "llvm-libc";
    needed = sizeof("llvm-libc");
    break;
  default:
    libc_errno = EINVAL;
    return 0;
  }

  // A length of zero is how a caller asks how much room it needs, so the
  // buffer is left alone and only the size is reported.
  if (buf != nullptr && len != 0) {
    size_t to_copy = len < needed ? len : needed;
    for (size_t i = 0; i + 1 < to_copy; ++i)
      buf[i] = value[i];
    buf[to_copy - 1] = '\0';
  }
  return needed;
}

} // namespace LIBC_NAMESPACE_DECL
