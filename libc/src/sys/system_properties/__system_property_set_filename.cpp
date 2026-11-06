//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation of __system_property_set_filename.
///
//===----------------------------------------------------------------------===//

#include "src/sys/system_properties/__system_property_set_filename.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/sys/system_properties/property_store.h"

namespace LIBC_NAMESPACE_DECL {

using namespace system_properties;

LLVM_LIBC_FUNCTION(int, __system_property_set_filename, (const char *filename)) {
  return set_filename(filename) ? 0 : -1;
}

} // namespace LIBC_NAMESPACE_DECL
