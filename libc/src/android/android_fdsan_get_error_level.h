//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation header for android_fdsan_get_error_level.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC_ANDROID_ANDROID_FDSAN_GET_ERROR_LEVEL_H
#define LLVM_LIBC_SRC_ANDROID_ANDROID_FDSAN_GET_ERROR_LEVEL_H

#include "hdr/types/enum_android_fdsan_error_level.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {

enum android_fdsan_error_level android_fdsan_get_error_level();

} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC_ANDROID_ANDROID_FDSAN_GET_ERROR_LEVEL_H
