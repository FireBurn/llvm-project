//===-- Implementation of setprotoent -------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/netdb/setprotoent.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/netdb/resolv/protocols.h"

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(void, setprotoent, (int stayopen)) {
  resolv::rewind_entries(stayopen != 0);
}

} // namespace LIBC_NAMESPACE_DECL
