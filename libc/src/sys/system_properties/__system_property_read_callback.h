//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation header for __system_property_read_callback.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC_SYS_SYSTEM_PROPERTIES_SYSTEM_PROPERTY_READ_CALLBACK_H
#define LLVM_LIBC_SRC_SYS_SYSTEM_PROPERTIES_SYSTEM_PROPERTY_READ_CALLBACK_H

#include "hdr/types/prop_info.h"
#include "hdr/types/__prop_read_callback_t.h"
#include "src/__support/macros/config.h"


namespace LIBC_NAMESPACE_DECL {

void __system_property_read_callback(const prop_info *info, __prop_read_callback_t callback, void *cookie);

} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC_SYS_SYSTEM_PROPERTIES_SYSTEM_PROPERTY_READ_CALLBACK_H
