//===-- Implementation of res_init ----------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/resolv/res_init.h"

#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/resolv/res_state.h"

namespace LIBC_NAMESPACE_DECL {

// Reads the configuration into the state _res names. Nothing has to call this
// any more, since every lookup that needs the state fills it in first, but
// programs written before that was so still do, and a program that has
// changed the file wants the new contents read.
LLVM_LIBC_FUNCTION(int, res_init, (void)) {
  return internal::res_setup(*LIBC_NAMESPACE::__res_state()) ? 0 : -1;
}

// The name the GNU library gives the same call. Rust's standard library asks
// for it after a lookup fails.
LLVM_LIBC_FUNCTION(int, __res_init, (void)) {
  return internal::res_setup(*LIBC_NAMESPACE::__res_state()) ? 0 : -1;
}

} // namespace LIBC_NAMESPACE_DECL
