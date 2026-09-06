//===-- Implementation of getusershell ------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/unistd/getusershell.h"

#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/unistd/usershell.h"

namespace LIBC_NAMESPACE_DECL {

// The next shell a user may be given as a login shell, or null at the end of
// the list. The string belongs to the library and the next call overwrites
// it.
LLVM_LIBC_FUNCTION(char *, getusershell, (void)) {
  return const_cast<char *>(usershell::next());
}

} // namespace LIBC_NAMESPACE_DECL
