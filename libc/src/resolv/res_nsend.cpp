//===-- Implementation of res_nsend ---------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/resolv/res_nsend.h"

#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/resolv/res_send.h"

namespace LIBC_NAMESPACE_DECL {

// res_send, with the state the caller keeps rather than the thread's own.
LLVM_LIBC_FUNCTION(int, res_nsend,
                   (struct __res_state * statp, const unsigned char *message,
                    int msglen, unsigned char *answer, int anslen)) {
  return internal::res_send_with(statp, message, msglen, answer, anslen);
}

} // namespace LIBC_NAMESPACE_DECL
