//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation header for __system_property_add.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC_SYS_SYSTEM_PROPERTIES_SYSTEM_PROPERTY_ADD_H
#define LLVM_LIBC_SRC_SYS_SYSTEM_PROPERTIES_SYSTEM_PROPERTY_ADD_H

#include "src/__support/macros/config.h"


namespace LIBC_NAMESPACE_DECL {

int __system_property_add(const char *name, unsigned int name_length, const char *value, unsigned int value_length);

} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC_SYS_SYSTEM_PROPERTIES_SYSTEM_PROPERTY_ADD_H
