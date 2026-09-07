//===-- The resolver's state ------------------------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC_RESOLV_RES_STATE_H
#define LLVM_LIBC_SRC_RESOLV_RES_STATE_H

#include "hdr/types/struct___res_state.h"
#include "src/__support/macros/config.h"
#include "src/netdb/resolv/resolv_conf.h"

namespace LIBC_NAMESPACE_DECL {

// The state _res names. It is per thread, so two threads looking things up at
// once do not tread on each other, which is what every library settled on
// after the single shared one proved impossible to use.
struct __res_state *__res_state(void);

namespace internal {

// Fills the state in from /etc/resolv.conf and marks it initialized. Returns
// false where there is nothing to ask.
bool res_setup(struct __res_state &state);

// Makes sure the state has been filled in, filling it in if not.
bool res_ready(struct __res_state &state);

// The configuration the state describes, in the form the sending code takes.
// The two are kept apart because a program may reach into the state and
// change what it says before asking anything.
void conf_from_state(const struct __res_state &state, resolv::ResolvConf &out);

} // namespace internal
} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC_RESOLV_RES_STATE_H
