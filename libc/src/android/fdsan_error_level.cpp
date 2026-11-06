//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Error level shared by the fdsan functions.
///
//===----------------------------------------------------------------------===//

#include "src/android/fdsan_error_level.h"
#include "hdr/types/enum_android_fdsan_error_level.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {
namespace fdsan {

cpp::Atomic<int> error_level(ANDROID_FDSAN_ERROR_LEVEL_DISABLED);
cpp::Atomic<bool> level_chosen(false);

} // namespace fdsan
} // namespace LIBC_NAMESPACE_DECL
