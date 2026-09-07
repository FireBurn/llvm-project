//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation of getservbyname
///
//===----------------------------------------------------------------------===//

#include "src/netdb/getservbyname.h"

#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/netdb/resolv/services.h"
#include "src/netdb/serv_storage.h"

namespace LIBC_NAMESPACE_DECL {

// The answer is kept in storage the library owns, which the next lookup from
// this thread overwrites. That is what this interface has always been.
LLVM_LIBC_FUNCTION(struct servent *, getservbyname,
                   (const char *name, const char *proto)) {
  return resolv::serv_by_name(name, proto, internal::serv_storage());
}

} // namespace LIBC_NAMESPACE_DECL
