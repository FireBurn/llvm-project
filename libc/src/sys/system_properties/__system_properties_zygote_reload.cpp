//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation of __system_properties_zygote_reload.
///
//===----------------------------------------------------------------------===//

#include "src/sys/system_properties/__system_properties_zygote_reload.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/stdlib/setenv.h"
#include "src/sys/system_properties/property_store.h"

namespace LIBC_NAMESPACE_DECL {

using namespace system_properties;

LLVM_LIBC_FUNCTION(int, __system_properties_zygote_reload, (void)) {
  State &s = state();
  s.appcompat_override = true;
  s.appcompat_override_checked = true;
  // The apps the zygote starts inherit the setting through their environment.
  return LIBC_NAMESPACE::setenv(APPCOMPAT_ENV, "1", 1) == 0 ? 0 : -1;
}

} // namespace LIBC_NAMESPACE_DECL
