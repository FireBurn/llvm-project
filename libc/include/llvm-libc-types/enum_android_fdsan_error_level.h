//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Definition of enum android_fdsan_error_level.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_TYPES_ENUM_ANDROID_FDSAN_ERROR_LEVEL_H
#define LLVM_LIBC_TYPES_ENUM_ANDROID_FDSAN_ERROR_LEVEL_H

enum android_fdsan_error_level {
  ANDROID_FDSAN_ERROR_LEVEL_DISABLED,
  ANDROID_FDSAN_ERROR_LEVEL_WARN_ONCE,
  ANDROID_FDSAN_ERROR_LEVEL_WARN_ALWAYS,
  ANDROID_FDSAN_ERROR_LEVEL_FATAL,
};

#endif // LLVM_LIBC_TYPES_ENUM_ANDROID_FDSAN_ERROR_LEVEL_H
