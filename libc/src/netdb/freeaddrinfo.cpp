//===-- Implementation of freeaddrinfo ------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/netdb/freeaddrinfo.h"
#include "hdr/func/free.h"
#include "hdr/types/struct_addrinfo.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {

// Each record was allocated with the address it points at, so giving the
// record back gives that back too. The name is its own allocation, and only
// the first record has one.
LLVM_LIBC_FUNCTION(void, freeaddrinfo, (struct addrinfo * res)) {
  for (struct addrinfo *entry = res; entry != nullptr;) {
    struct addrinfo *next = entry->ai_next;
    ::free(entry->ai_canonname);
    ::free(entry);
    entry = next;
  }
}

} // namespace LIBC_NAMESPACE_DECL
