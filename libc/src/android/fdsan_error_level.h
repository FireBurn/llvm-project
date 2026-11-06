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

#ifndef LLVM_LIBC_SRC_ANDROID_FDSAN_ERROR_LEVEL_H
#define LLVM_LIBC_SRC_ANDROID_FDSAN_ERROR_LEVEL_H

#include "src/__support/CPP/atomic.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {
namespace fdsan {

// An android_fdsan_error_level. Descriptors are not tracked, so the level is
// only kept for callers to read back.
extern cpp::Atomic<int> error_level;

// Whether set_error_level_from_property has already chosen a level.
extern cpp::Atomic<bool> level_chosen;

} // namespace fdsan
} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC_ANDROID_FDSAN_ERROR_LEVEL_H
