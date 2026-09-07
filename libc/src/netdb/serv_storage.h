//===-- Where a service entry is handed back from ---------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC_NETDB_SERV_STORAGE_H
#define LLVM_LIBC_SRC_NETDB_SERV_STORAGE_H

#include "src/__support/macros/config.h"
#include "src/netdb/resolv/services.h"

namespace LIBC_NAMESPACE_DECL {
namespace internal {

// The lookups share one of these per thread, since each replaces what the
// last handed back.
resolv::ServStorage &serv_storage();

} // namespace internal
} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC_NETDB_SERV_STORAGE_H
