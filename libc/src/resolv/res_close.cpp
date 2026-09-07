//===-- Implementation of res_close ---------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/resolv/res_close.h"

#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/resolv/res_state.h"
#include "src/unistd/close.h"

namespace LIBC_NAMESPACE_DECL {

// Lets go of whatever the resolver was holding open. Each question is asked
// over a socket of its own here, so there is only ever the connection
// RES_STAYOPEN asked to be kept.
LLVM_LIBC_FUNCTION(void, res_close, (void)) {
  struct __res_state *state = LIBC_NAMESPACE::__res_state();
  if (state->_vcsock >= 0) {
    LIBC_NAMESPACE::close(state->_vcsock);
    state->_vcsock = -1;
  }
}

} // namespace LIBC_NAMESPACE_DECL
