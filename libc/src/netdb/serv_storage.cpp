//===-- Where a service entry is handed back from -------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/netdb/serv_storage.h"
#include "src/__support/macros/attributes.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {
namespace internal {

resolv::ServStorage &serv_storage() {
  static LIBC_THREAD_LOCAL resolv::ServStorage storage;
  return storage;
}

} // namespace internal
} // namespace LIBC_NAMESPACE_DECL
