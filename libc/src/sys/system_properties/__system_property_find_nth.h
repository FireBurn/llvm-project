//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation header for __system_property_find_nth.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC_SYS_SYSTEM_PROPERTIES_SYSTEM_PROPERTY_FIND_NTH_H
#define LLVM_LIBC_SRC_SYS_SYSTEM_PROPERTIES_SYSTEM_PROPERTY_FIND_NTH_H

#include "hdr/types/prop_info.h"
#include "src/__support/macros/config.h"


namespace LIBC_NAMESPACE_DECL {

const prop_info *__system_property_find_nth(unsigned n);

} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC_SYS_SYSTEM_PROPERTIES_SYSTEM_PROPERTY_FIND_NTH_H
