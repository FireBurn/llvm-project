//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation of android_set_application_target_sdk_version.
///
//===----------------------------------------------------------------------===//

#include "src/android/android_set_application_target_sdk_version.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/android/target_sdk_version.h"

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(void, android_set_application_target_sdk_version,
                   (int target)) {
  android::set_target_sdk_version(target);
}

} // namespace LIBC_NAMESPACE_DECL
