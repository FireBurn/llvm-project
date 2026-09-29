//===-- Implementation of ns_skiprr ---------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/arpa/nameser/ns_skiprr.h"

#include "hdr/errno_macros.h"
#include "hdr/types/size_t.h"
#include "src/__support/common.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"
#include "src/netdb/resolv/dns_message.h"

namespace LIBC_NAMESPACE_DECL {

// Steps over `count` records of `section` starting at `ptr`, and returns how
// many bytes they took. A name is not followed where it points elsewhere in
// the message, so the start of the message is not needed.
LLVM_LIBC_FUNCTION(int, ns_skiprr,
                   (const unsigned char *ptr, const unsigned char *eom,
                    int section, int count)) {
  if (ptr == nullptr || eom < ptr) {
    libc_errno = EMSGSIZE;
    return -1;
  }
  const size_t length = static_cast<size_t>(eom - ptr);
  size_t at = 0;
  for (; count > 0; --count) {
    at = resolv::skip_name(ptr, length, at);
    if (at == 0) {
      libc_errno = EMSGSIZE;
      return -1;
    }
    // A question is only its type and class; a record adds a time to live
    // and its data, which is as long as the two bytes before it say.
    if (section == 0) {
      at += 4;
    } else {
      at += 8;
      if (at + 2 > length) {
        libc_errno = EMSGSIZE;
        return -1;
      }
      at += 2 + ((static_cast<size_t>(ptr[at]) << 8) | ptr[at + 1]);
    }
    if (at > length) {
      libc_errno = EMSGSIZE;
      return -1;
    }
  }
  return static_cast<int>(at);
}

} // namespace LIBC_NAMESPACE_DECL
