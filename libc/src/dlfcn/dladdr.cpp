//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation of dladdr
///
//===----------------------------------------------------------------------===//

#include "dladdr.h"

#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/dlfcn/dl_address.h"

namespace LIBC_NAMESPACE_DECL {

// Says which object an address is in, and which symbol of it. Unlike the rest
// of dlfcn this reports failure with zero rather than with a message, and it
// leaves dlerror alone, which is what every other implementation does.
namespace internal {
int dladdr_impl(const void *__restrict addr, Dl_info *__restrict info) {
  return dl::address_lookup(addr, info, nullptr, nullptr) ? 1 : 0;
}

} // namespace internal

LLVM_LIBC_FUNCTION(int, dladdr,
                   (const void *__restrict addr, Dl_info *__restrict info)) {
  return internal::dladdr_impl(addr, info);
}

} // namespace LIBC_NAMESPACE_DECL
