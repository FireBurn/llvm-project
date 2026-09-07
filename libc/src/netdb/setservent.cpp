//===-- Implementation of setservent --------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/netdb/setservent.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/netdb/resolv/services.h"

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(void, setservent, (int stayopen)) {
  resolv::rewind_services(stayopen != 0);
}

} // namespace LIBC_NAMESPACE_DECL
