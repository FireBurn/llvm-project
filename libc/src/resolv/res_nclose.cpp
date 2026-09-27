//===-- Implementation of res_nclose --------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/resolv/res_nclose.h"

#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/resolv/res_close.h"

namespace LIBC_NAMESPACE_DECL {

// Closes what a state the caller keeps holds open.
LLVM_LIBC_FUNCTION(void, res_nclose, (struct __res_state * statp)) {
  internal::res_close_with(statp);
}

} // namespace LIBC_NAMESPACE_DECL
