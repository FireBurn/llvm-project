//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation header for __system_property_foreach.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC_SYS_SYSTEM_PROPERTIES_SYSTEM_PROPERTY_FOREACH_H
#define LLVM_LIBC_SRC_SYS_SYSTEM_PROPERTIES_SYSTEM_PROPERTY_FOREACH_H

#include "hdr/types/__prop_foreach_callback_t.h"
#include "hdr/types/prop_info.h"
#include "src/__support/macros/config.h"


namespace LIBC_NAMESPACE_DECL {

int __system_property_foreach(__prop_foreach_callback_t callback, void *cookie);

} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC_SYS_SYSTEM_PROPERTIES_SYSTEM_PROPERTY_FOREACH_H
