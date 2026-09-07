//===-- Where a host entry is handed back from ------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC_NETDB_HOSTS_STORAGE_H
#define LLVM_LIBC_SRC_NETDB_HOSTS_STORAGE_H

#include "src/__support/macros/config.h"
#include "src/netdb/resolv/hosts.h"

namespace LIBC_NAMESPACE_DECL {
namespace internal {

// gethostent hands back what it read out of the file, which the next read
// replaces, so there is one of these per thread. It is separate from the
// storage gethostbyname uses, since walking the file must not disturb an
// answer a lookup handed back.
resolv::HostsStorage &hosts_storage();

} // namespace internal
} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC_NETDB_HOSTS_STORAGE_H
