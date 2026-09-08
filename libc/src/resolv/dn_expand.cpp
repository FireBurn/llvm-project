//===-- Implementation of dn_expand ---------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/resolv/dn_expand.h"

#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/netdb/resolv/dns_message.h"

namespace LIBC_NAMESPACE_DECL {

// Reads the name at `src` out of the message running from `msg` to `eom`,
// following the pointers the format uses in place of repeating a name that
// has already appeared. Returns how many bytes of the message the name took
// where it was written, which is two where it was only a pointer.
LLVM_LIBC_FUNCTION(int, dn_expand,
                   (const unsigned char *msg, const unsigned char *eom,
                    const unsigned char *src, char *dn, int dnsiz)) {
  if (msg == nullptr || eom == nullptr || src == nullptr || dn == nullptr)
    return -1;
  if (dnsiz <= 0 || eom < msg || src < msg || src >= eom)
    return -1;

  const size_t length = static_cast<size_t>(eom - msg);
  const size_t at = static_cast<size_t>(src - msg);
  const size_t used =
      resolv::read_name(msg, length, at, dn, static_cast<size_t>(dnsiz));
  if (used == 0)
    return -1;

  // The root on its own is written as a single dot, which is the one name
  // read_name leaves empty.
  if (dn[0] == '\0' && dnsiz >= 2) {
    dn[0] = '.';
    dn[1] = '\0';
  }
  return static_cast<int>(used);
}

} // namespace LIBC_NAMESPACE_DECL
