//===-- Implementation header of res_send ---------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC_RESOLV_RES_SEND_H
#define LLVM_LIBC_SRC_RESOLV_RES_SEND_H

#include "src/__support/macros/config.h"

struct __res_state;

namespace LIBC_NAMESPACE_DECL {

int res_send(const unsigned char *message, int msglen, unsigned char *answer,
             int anslen);

namespace internal {
int res_send_with(struct __res_state *state, const unsigned char *message,
                  int msglen, unsigned char *answer, int anslen);
} // namespace internal

} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC_RESOLV_RES_SEND_H
