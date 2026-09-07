//===-- Implementation of getnetbyname ------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/netdb/getnetbyname.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/netdb/net_storage.h"
#include "src/netdb/resolv/networks.h"

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(struct netent *, getnetbyname, (const char *name)) {
  return resolv::net_by_name(name, internal::net_storage());
}

} // namespace LIBC_NAMESPACE_DECL
