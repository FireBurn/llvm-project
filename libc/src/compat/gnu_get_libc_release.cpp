//===-- Implementation of gnu_get_libc_release ----------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/compat/gnu_get_libc_release.h"

#include "src/__support/common.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {

// glibc reports how its own release was made, and says "stable" for one that
// was released rather than built from a development tree. Nothing reads this
// to decide anything, and there is nothing else truthful to say.
LLVM_LIBC_FUNCTION(const char *, gnu_get_libc_release, (void)) {
  return "stable";
}

} // namespace LIBC_NAMESPACE_DECL
