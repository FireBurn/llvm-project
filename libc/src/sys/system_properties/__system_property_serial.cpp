//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation of __system_property_serial.
///
//===----------------------------------------------------------------------===//

#include "src/sys/system_properties/__system_property_serial.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/sys/system_properties/property_store.h"

namespace LIBC_NAMESPACE_DECL {

using namespace system_properties;

LLVM_LIBC_FUNCTION(uint32_t, __system_property_serial, (const prop_info *info)) {
  const PropInfo *property = reinterpret_cast<const PropInfo *>(info);
  Word &word = const_cast<Word &>(property->serial);
  uint32_t serial = word.load(cpp::MemoryOrder::ACQUIRE);
  // While a value is being changed its serial is not one to hand out.
  while (serial_dirty(serial)) {
    uint32_t next = serial;
    if (wait(property, serial, &next, nullptr))
      serial = next;
    else
      serial = word.load(cpp::MemoryOrder::ACQUIRE);
  }
  return serial;
}

} // namespace LIBC_NAMESPACE_DECL
