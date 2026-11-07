//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation of __system_property_find.
///
//===----------------------------------------------------------------------===//

#include "src/sys/system_properties/__system_property_find.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/sys/system_properties/property_store.h"

namespace LIBC_NAMESPACE_DECL {

using namespace system_properties;

LLVM_LIBC_FUNCTION(const prop_info *, __system_property_find, (const char *name)) {
  Area *a = area_for(name);
  if (a == nullptr)
    return nullptr;
  return reinterpret_cast<const prop_info *>(a->find(name));
}

} // namespace LIBC_NAMESPACE_DECL
