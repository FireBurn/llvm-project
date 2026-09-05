//===-- Linux implementation of ptsname -----------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/stdlib/ptsname.h"

#include "src/__support/common.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"
#include "src/stdlib/linux/pty_utils.h"
#include "src/stdlib/ptsname_r.h"

namespace LIBC_NAMESPACE_DECL {

// The name goes in storage the library keeps, which the next call
// overwrites.
LLVM_LIBC_FUNCTION(char *, ptsname, (int fd)) {
  // Room for the prefix, the widest number and the terminator.
  static char name[pty::PTS_PREFIX_LEN + 11];
  int err = LIBC_NAMESPACE::ptsname_r(fd, name, sizeof(name));
  if (err != 0) {
    libc_errno = err;
    return nullptr;
  }
  return name;
}

} // namespace LIBC_NAMESPACE_DECL
