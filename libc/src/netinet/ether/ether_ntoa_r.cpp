//===-- Implementation of ether_ntoa_r ------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/netinet/ether/ether_ntoa_r.h"

#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/netinet/ether/ethers.h"

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(char *, ether_ntoa_r,
                   (const struct ether_addr *addr, char *buf)) {
  ether::to_text(addr, buf);
  return buf;
}

} // namespace LIBC_NAMESPACE_DECL
