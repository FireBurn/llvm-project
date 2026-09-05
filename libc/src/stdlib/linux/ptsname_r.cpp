//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Linux implementation of ptsname_r
///
//===----------------------------------------------------------------------===//

#include "src/stdlib/ptsname_r.h"

#include "hdr/types/size_t.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/stdlib/linux/pty_utils.h"

namespace LIBC_NAMESPACE_DECL {

// Returns the error rather than setting errno, which is what the reentrant
// form of this is expected to do.
LLVM_LIBC_FUNCTION(int, ptsname_r, (int fd, char *buf, size_t buflen)) {
  return pty::name(fd, buf, buflen);
}

} // namespace LIBC_NAMESPACE_DECL
