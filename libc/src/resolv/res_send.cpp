//===-- Implementation of res_send ----------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/resolv/res_send.h"

#include "src/__support/common.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"
#include "src/netdb/resolv/exchange.h"
#include "src/netdb/resolv/resolv_conf.h"
#include "src/resolv/res_state.h"

namespace LIBC_NAMESPACE_DECL {

// Sends a message that has already been written and hands back what came
// back, without reading either. What the answer means is the caller's to
// work out.
LLVM_LIBC_FUNCTION(int, res_send,
                   (const unsigned char *message, int msglen,
                    unsigned char *answer, int anslen)) {
  if (message == nullptr || answer == nullptr || msglen <= 0 || anslen <= 0) {
    libc_errno = EINVAL;
    return -1;
  }

  struct __res_state *state = LIBC_NAMESPACE::__res_state();
  if (!internal::res_ready(*state)) {
    libc_errno = ECONNREFUSED;
    return -1;
  }

  resolv::ResolvConf conf;
  internal::conf_from_state(*state, conf);
  if (conf.server_count == 0) {
    libc_errno = ECONNREFUSED;
    return -1;
  }

  const int length =
      resolv::exchange(conf, message, static_cast<size_t>(msglen), answer,
                       static_cast<size_t>(anslen));
  if (length < 0) {
    libc_errno = ECONNREFUSED;
    return -1;
  }
  return length;
}

} // namespace LIBC_NAMESPACE_DECL
