//===-- Implementation of res_ninit ---------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/resolv/res_ninit.h"

#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/resolv/res_state.h"

namespace LIBC_NAMESPACE_DECL {

// Reads the configuration into a state the caller keeps.
LLVM_LIBC_FUNCTION(int, res_ninit, (struct __res_state * statp)) {
  return internal::res_setup(*statp) ? 0 : -1;
}

} // namespace LIBC_NAMESPACE_DECL
