//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation header for android_set_application_target_sdk_version.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC_ANDROID_ANDROID_SET_APPLICATION_TARGET_SDK_VERSION_H
#define LLVM_LIBC_SRC_ANDROID_ANDROID_SET_APPLICATION_TARGET_SDK_VERSION_H

#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {

void android_set_application_target_sdk_version(int target);

} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC_ANDROID_ANDROID_SET_APPLICATION_TARGET_SDK_VERSION_H
