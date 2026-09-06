//===-- Implementation of getprotobyname ----------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/netdb/getprotobyname.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/netdb/proto_storage.h"
#include "src/netdb/resolv/protocols.h"

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(struct protoent *, getprotobyname, (const char *name)) {
  return resolv::by_name(name, internal::proto_storage());
}

} // namespace LIBC_NAMESPACE_DECL
