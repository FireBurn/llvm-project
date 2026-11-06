//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation of __system_property_read_callback.
///
//===----------------------------------------------------------------------===//

#include "src/sys/system_properties/__system_property_read_callback.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/sys/system_properties/property_store.h"

namespace LIBC_NAMESPACE_DECL {

using namespace system_properties;

LLVM_LIBC_FUNCTION(void, __system_property_read_callback, (const prop_info *info, __prop_read_callback_t callback, void *cookie)) {
  const PropInfo *property = reinterpret_cast<const PropInfo *>(info);
  Word &word = const_cast<Word &>(property->serial);
  if (property->is_long()) {
    callback(cookie, property->name(), property->long_value(),
             word.load(cpp::MemoryOrder::ACQUIRE));
    return;
  }
  char value[VALUE_MAX];
  // Read the serial first so the callback is told the one the value goes with.
  uint32_t serial = word.load(cpp::MemoryOrder::ACQUIRE);
  while (true) {
    read_value(property, value);
    const uint32_t again = word.load(cpp::MemoryOrder::ACQUIRE);
    if (again == serial)
      break;
    serial = again;
  }
  callback(cookie, property->name(), value, serial);
}

} // namespace LIBC_NAMESPACE_DECL
