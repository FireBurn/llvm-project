//===-- Implementation of ns_name_uncompress ------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/arpa/nameser/ns_name_uncompress.h"

#include "hdr/errno_macros.h"
#include "src/__support/common.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"
#include "src/netdb/resolv/dns_message.h"

namespace LIBC_NAMESPACE_DECL {

// dn_expand under the name BIND gave it later, which says what went wrong.
LLVM_LIBC_FUNCTION(int, ns_name_uncompress,
                   (const unsigned char *msg, const unsigned char *eom,
                    const unsigned char *src, char *dst, size_t dstsiz)) {
  if (msg == nullptr || eom == nullptr || src == nullptr || dst == nullptr ||
      eom < msg || src < msg || src >= eom || dstsiz < 2) {
    libc_errno = EMSGSIZE;
    return -1;
  }
  const size_t used =
      resolv::read_name(msg, static_cast<size_t>(eom - msg),
                        static_cast<size_t>(src - msg), dst, dstsiz);
  if (used == 0) {
    libc_errno = EMSGSIZE;
    return -1;
  }
  if (dst[0] == '\0') {
    dst[0] = '.';
    dst[1] = '\0';
  }
  return static_cast<int>(used);
}

} // namespace LIBC_NAMESPACE_DECL
