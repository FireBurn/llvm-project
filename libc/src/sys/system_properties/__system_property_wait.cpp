//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation of __system_property_wait.
///
//===----------------------------------------------------------------------===//

#include "src/sys/system_properties/__system_property_wait.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/sys/system_properties/property_store.h"

namespace LIBC_NAMESPACE_DECL {

using namespace system_properties;

LLVM_LIBC_FUNCTION(bool, __system_property_wait, (const prop_info *info, uint32_t old_serial, uint32_t *new_serial, const struct timespec *relative_timeout)) {
  return wait(reinterpret_cast<const PropInfo *>(info), old_serial, new_serial,
              relative_timeout);
}

} // namespace LIBC_NAMESPACE_DECL
