//===-- Implementation of res_mkquery -------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/resolv/res_mkquery.h"

#include "hdr/stdint_proxy.h"
#include "src/__support/OSUtil/linux/syscall_wrappers/getrandom.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/netdb/resolv/dns_message.h"
#include "src/resolv/res_state.h"

namespace LIBC_NAMESPACE_DECL {

// Writes the question that would be sent to ask about |dname|, without
// sending it. |data| and |newrr| belong to the update opcode, which is not
// written here.
LLVM_LIBC_FUNCTION(int, res_mkquery,
                   (int op, const char *dname, int rr_class, int type,
                    const unsigned char *data, int datalen,
                    const unsigned char *newrr, unsigned char *buf,
                    int buflen)) {
  (void)data;
  (void)datalen;
  (void)newrr;
  if (op != resolv::OPCODE_QUERY || dname == nullptr || buf == nullptr ||
      buflen <= 0)
    return -1;

  struct __res_state *state = LIBC_NAMESPACE::__res_state();
  internal::res_ready(*state);

  // Each question carries an identifier the answer must repeat, so that a
  // reply to somebody else's question is not taken for this one.
  uint16_t id = 0;
  if (!linux_syscalls::getrandom(&id, sizeof(id), 0).has_value())
    return -1;
  state->id = id;

  const size_t length = resolv::build_query(dname, static_cast<uint16_t>(type),
                                            static_cast<uint16_t>(rr_class), id,
                                            buf, static_cast<size_t>(buflen));
  return length == 0 ? -1 : static_cast<int>(length);
}

} // namespace LIBC_NAMESPACE_DECL
