//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation header for __system_property_update.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC_SYS_SYSTEM_PROPERTIES_SYSTEM_PROPERTY_UPDATE_H
#define LLVM_LIBC_SRC_SYS_SYSTEM_PROPERTIES_SYSTEM_PROPERTY_UPDATE_H

#include "hdr/types/prop_info.h"
#include "src/__support/macros/config.h"


namespace LIBC_NAMESPACE_DECL {

int __system_property_update(prop_info *info, const char *value, unsigned int value_length);

} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC_SYS_SYSTEM_PROPERTIES_SYSTEM_PROPERTY_UPDATE_H
