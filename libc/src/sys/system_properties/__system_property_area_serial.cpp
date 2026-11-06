//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation of __system_property_area_serial.
///
//===----------------------------------------------------------------------===//

#include "src/sys/system_properties/__system_property_area_serial.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/sys/system_properties/property_store.h"

namespace LIBC_NAMESPACE_DECL {

using namespace system_properties;

LLVM_LIBC_FUNCTION(uint32_t, __system_property_area_serial, (void)) {
  Area *a = area();
  if (a == nullptr)
    return static_cast<uint32_t>(-1);
  return a->serial().load(cpp::MemoryOrder::ACQUIRE);
}

} // namespace LIBC_NAMESPACE_DECL
