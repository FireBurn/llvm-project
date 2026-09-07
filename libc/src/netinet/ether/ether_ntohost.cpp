//===-- Implementation of ether_ntohost -----------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/netinet/ether/ether_ntohost.h"

#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/netinet/ether/ethers.h"

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(int, ether_ntohost,
                   (char *hostname, const struct ether_addr *addr)) {
  return ether::find_by_addr(addr, hostname) ? 0 : -1;
}

} // namespace LIBC_NAMESPACE_DECL
