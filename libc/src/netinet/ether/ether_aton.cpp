//===-- Implementation of ether_aton --------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/netinet/ether/ether_aton.h"

#include "src/__support/common.h"
#include "src/__support/macros/attributes.h"
#include "src/__support/macros/config.h"
#include "src/netinet/ether/ethers.h"

namespace LIBC_NAMESPACE_DECL {

namespace {
LIBC_CONSTINIT LIBC_THREAD_LOCAL struct ether_addr result = {};
} // namespace

// The answer is kept in storage the library owns, which the next call from
// this thread overwrites.
LLVM_LIBC_FUNCTION(struct ether_addr *, ether_aton, (const char *asc)) {
  return ether::from_text(asc, &result) ? &result : nullptr;
}

} // namespace LIBC_NAMESPACE_DECL
