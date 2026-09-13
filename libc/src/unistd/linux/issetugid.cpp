//===-- Linux implementation of issetugid ---------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/unistd/issetugid.h"

#include "src/__support/OSUtil/linux/auxv.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {

// The kernel marks a process AT_SECURE when running it changed its identity
// or gave it capabilities, as a set-user-ID program does, and that is the
// question issetugid answers. A process that cannot read the mark is told
// that it is set, which is the safe answer for a caller deciding whether to
// trust its environment.
LLVM_LIBC_FUNCTION(int, issetugid, ()) {
  cpp::optional<unsigned long> secure = auxv::get(AT_SECURE);
  if (!secure)
    return 1;
  return *secure != 0 ? 1 : 0;
}

} // namespace LIBC_NAMESPACE_DECL
