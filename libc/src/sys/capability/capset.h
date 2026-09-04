//===-- Implementation header for capset ------------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC_SYS_CAPABILITY_CAPSET_H
#define LLVM_LIBC_SRC_SYS_CAPABILITY_CAPSET_H

#include "src/__support/macros/config.h"
#include "src/sys/capability/cap_types.h"

namespace LIBC_NAMESPACE_DECL {

int capset(cap_user_header_t header, const cap_user_data_t data);

} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC_SYS_CAPABILITY_CAPSET_H
