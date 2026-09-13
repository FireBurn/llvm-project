//===-- Implementation header of inet_makeaddr ------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC_ARPA_INET_INET_MAKEADDR_H
#define LLVM_LIBC_SRC_ARPA_INET_INET_MAKEADDR_H

#include "hdr/types/in_addr_t.h"
#include "hdr/types/struct_in_addr.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {

struct in_addr inet_makeaddr(in_addr_t net, in_addr_t host);

} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC_ARPA_INET_INET_MAKEADDR_H
