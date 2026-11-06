//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation of __system_property_get.
///
//===----------------------------------------------------------------------===//

#include "src/sys/system_properties/__system_property_get.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/sys/system_properties/property_store.h"

namespace LIBC_NAMESPACE_DECL {

using namespace system_properties;

LLVM_LIBC_FUNCTION(int, __system_property_get, (const char *name, char *value)) {
  Area *a = area();
  const PropInfo *property = a == nullptr ? nullptr : a->find(name);
  if (property == nullptr) {
    value[0] = '\0';
    return 0;
  }
  if (property->is_long()) {
    inline_memcpy(value, property->long_property.error_message,
                  sizeof(LONG_MESSAGE));
    return static_cast<int>(sizeof(LONG_MESSAGE) - 1);
  }
  return static_cast<int>(read_value(property, value));
}

} // namespace LIBC_NAMESPACE_DECL
