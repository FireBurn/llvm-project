//===-- Implementation of setusershell ------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/unistd/setusershell.h"

#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/unistd/usershell.h"

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(void, setusershell, (void)) { usershell::rewind(); }

} // namespace LIBC_NAMESPACE_DECL
