//===-- Implementation of inet_network function ---------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/arpa/inet/inet_network.h"
#include "hdr/netinet_in_macros.h"
#include "hdr/types/in_addr_t.h"
#include "src/__support/CPP/optional.h"
#include "src/__support/common.h"
#include "src/__support/macros/null_check.h"
#include "src/__support/net/address.h"

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(in_addr_t, inet_network, (const char *cp)) {
  LIBC_CRASH_ON_NULLPTR(cp);
  cpp::optional<in_addr_t> network = net::inet_network(cp);
  if (!network.has_value())
    return INADDR_NONE;
  return network.value();
}

} // namespace LIBC_NAMESPACE_DECL
