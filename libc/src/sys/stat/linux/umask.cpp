//===-- Implementation of umask -------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/sys/stat/umask.h"

#include "src/__support/OSUtil/syscall.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"

#include <sys/syscall.h>

namespace LIBC_NAMESPACE_DECL {

// The syscall cannot fail, so there is no errno to set: it always returns the
// previous mask.
LLVM_LIBC_FUNCTION(mode_t, umask, (mode_t cmask)) {
  return static_cast<mode_t>(
      LIBC_NAMESPACE::syscall_impl<int>(SYS_umask, cmask));
}

} // namespace LIBC_NAMESPACE_DECL
