//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation of __system_property_find_nth.
///
//===----------------------------------------------------------------------===//

#include "src/sys/system_properties/__system_property_find_nth.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/sys/system_properties/property_store.h"

namespace LIBC_NAMESPACE_DECL {

using namespace system_properties;

LLVM_LIBC_FUNCTION(const prop_info *, __system_property_find_nth, (unsigned n)) {
  Area *a = area();
  if (a == nullptr)
    return nullptr;
  const PropInfo *found = nullptr;
  unsigned index = 0;
  a->foreach ([&](const PropInfo *info) {
    if (index++ == n)
      found = info;
  });
  return reinterpret_cast<const prop_info *>(found);
}

} // namespace LIBC_NAMESPACE_DECL
