//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation of android_get_device_api_level.
///
//===----------------------------------------------------------------------===//

#include "src/android/android_get_device_api_level.h"
#include "hdr/system_properties_macros.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/sys/system_properties/property_store.h"

namespace LIBC_NAMESPACE_DECL {

// The level is the decimal ro.build.version.sdk property; -1 says there is
// none, or that it is not a positive number.
LLVM_LIBC_FUNCTION(int, android_get_device_api_level, ()) {
  using namespace system_properties;
  // find_property, not the area's own find: on a device the property is in the
  // area of its SELinux context, which only the store knows how to pick.
  const PropInfo *property = find_property("ro.build.version.sdk");
  if (property == nullptr || property->is_long())
    return -1;
  char value[PROP_VALUE_MAX];
  read_value(property, value);
  int level = 0;
  for (const char *c = value; *c != '\0'; ++c) {
    if (*c < '0' || *c > '9' || level > 100000)
      return -1;
    level = level * 10 + (*c - '0');
  }
  return level > 0 ? level : -1;
}

} // namespace LIBC_NAMESPACE_DECL
