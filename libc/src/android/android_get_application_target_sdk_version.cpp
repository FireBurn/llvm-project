//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation of android_get_application_target_sdk_version.
///
//===----------------------------------------------------------------------===//

#include "src/android/android_get_application_target_sdk_version.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/android/target_sdk_version.h"

namespace LIBC_NAMESPACE_DECL {

// Only an Android application has a target level to report, and it says so
// with android_set_application_target_sdk_version. A native program is treated
// as built for the newest, as it is on Android.
LLVM_LIBC_FUNCTION(int, android_get_application_target_sdk_version, ()) {
  return android::target_sdk_version();
}

} // namespace LIBC_NAMESPACE_DECL
