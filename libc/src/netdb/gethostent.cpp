//===-- Implementation of gethostent --------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/netdb/gethostent.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/netdb/hosts_storage.h"
#include "src/netdb/resolv/hosts.h"

namespace LIBC_NAMESPACE_DECL {

// The answer is kept in storage the library owns, which the next read from
// this thread overwrites. That is what this interface has always been.
LLVM_LIBC_FUNCTION(struct hostent *, gethostent, (void)) {
  return resolv::next_host(internal::hosts_storage());
}

} // namespace LIBC_NAMESPACE_DECL
