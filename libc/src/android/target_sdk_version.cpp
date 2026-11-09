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

#include "src/android/target_sdk_version.h"
#include "hdr/android_api_level_macros.h"
#include "src/__support/CPP/atomic.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {
namespace android {

namespace {
cpp::Atomic<int> version(__ANDROID_API_FUTURE__);
} // anonymous namespace

int target_sdk_version() { return version.load(cpp::MemoryOrder::RELAXED); }

void set_target_sdk_version(int value) {
  version.store(value, cpp::MemoryOrder::RELAXED);
}

} // namespace android
} // namespace LIBC_NAMESPACE_DECL
