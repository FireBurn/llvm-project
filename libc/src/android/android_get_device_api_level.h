//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation header for android_get_device_api_level.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC_ANDROID_ANDROID_GET_DEVICE_API_LEVEL_H
#define LLVM_LIBC_SRC_ANDROID_ANDROID_GET_DEVICE_API_LEVEL_H

#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {

int android_get_device_api_level();

} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC_ANDROID_ANDROID_GET_DEVICE_API_LEVEL_H
