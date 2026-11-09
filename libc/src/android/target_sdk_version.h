//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// The target level an application declared.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC_ANDROID_TARGET_SDK_VERSION_H
#define LLVM_LIBC_SRC_ANDROID_TARGET_SDK_VERSION_H

#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {
namespace android {

// What android_set_application_target_sdk_version was given, or the newest
// level until it was.
int target_sdk_version();
void set_target_sdk_version(int version);

} // namespace android
} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC_ANDROID_TARGET_SDK_VERSION_H
