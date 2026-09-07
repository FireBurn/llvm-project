//===-- Implementation of res_query ---------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/resolv/res_query.h"

#include "hdr/netdb_macros.h"
#include "hdr/stdint_proxy.h"
#include "src/__support/OSUtil/linux/syscall_wrappers/getrandom.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/netdb/__h_errno_location.h"
#include "src/netdb/resolv/dns_message.h"
#include "src/netdb/resolv/exchange.h"
#include "src/netdb/resolv/resolv_conf.h"
#include "src/resolv/res_state.h"

namespace LIBC_NAMESPACE_DECL {

// Asks the name servers about one name and hands back the whole answer as it
// arrived. What is in it is the caller's to read: this is the way in for code
// that wants a record kind the library knows nothing about.
LLVM_LIBC_FUNCTION(int, res_query,
                   (const char *name, int rr_class, int type,
                    unsigned char *answer, int anslen)) {
  int *h_errno_location = LIBC_NAMESPACE::__h_errno_location();
  if (name == nullptr || answer == nullptr || anslen <= 0) {
    *h_errno_location = NO_RECOVERY;
    return -1;
  }

  struct __res_state *state = LIBC_NAMESPACE::__res_state();
  if (!internal::res_ready(*state)) {
    *h_errno_location = NO_RECOVERY;
    return -1;
  }

  resolv::ResolvConf conf;
  internal::conf_from_state(*state, conf);
  if (conf.server_count == 0) {
    *h_errno_location = TRY_AGAIN;
    return -1;
  }

  uint16_t id = 0;
  if (!linux_syscalls::getrandom(&id, sizeof(id), 0).has_value()) {
    *h_errno_location = NO_RECOVERY;
    return -1;
  }
  state->id = id;

  unsigned char message[resolv::MAX_MESSAGE];
  const size_t length = resolv::build_query(name, static_cast<uint16_t>(type),
                                            static_cast<uint16_t>(rr_class), id,
                                            message, sizeof(message));
  if (length == 0) {
    *h_errno_location = NO_RECOVERY;
    return -1;
  }

  const int taken = resolv::exchange(conf, message, length, answer,
                                     static_cast<size_t>(anslen));
  if (taken < 0) {
    *h_errno_location = TRY_AGAIN;
    return -1;
  }

  // The four low bits of the second header byte say what the server made of
  // the question. Only two of the answers are worth telling apart here: the
  // name is not there, or something else went wrong.
  if (taken >= 4) {
    const unsigned rcode = static_cast<unsigned>(answer[3] & 0x0f);
    const unsigned answers =
        (static_cast<unsigned>(answer[6]) << 8) | answer[7];
    if (rcode == 3) {
      *h_errno_location = HOST_NOT_FOUND;
      return -1;
    }
    if (rcode != 0) {
      *h_errno_location = rcode == 2 ? TRY_AGAIN : NO_RECOVERY;
      return -1;
    }
    if (answers == 0) {
      *h_errno_location = NO_DATA;
      return -1;
    }
  }

  return taken;
}

} // namespace LIBC_NAMESPACE_DECL
