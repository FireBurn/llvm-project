//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation of __system_property_update.
///
//===----------------------------------------------------------------------===//

#include "src/sys/system_properties/__system_property_update.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/sys/system_properties/property_store.h"

namespace LIBC_NAMESPACE_DECL {

using namespace system_properties;

LLVM_LIBC_FUNCTION(int, __system_property_update, (prop_info *info, const char *value, unsigned int value_length)) {
  return update(reinterpret_cast<PropInfo *>(info), value, value_length);
}

} // namespace LIBC_NAMESPACE_DECL
