//===-- Implementation header of setservent ----------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC_NETDB_SETSERVENT_H
#define LLVM_LIBC_SRC_NETDB_SETSERVENT_H

#include "hdr/types/struct_servent.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {

void setservent(int stayopen);

} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC_NETDB_SETSERVENT_H
