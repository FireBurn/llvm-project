//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation of android_crash_detail_unregister.
///
//===----------------------------------------------------------------------===//

#include "src/android/android_crash_detail_unregister.h"
#include "src/__support/common.h"
#include "src/__support/crash_detail.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(void, android_crash_detail_unregister,
                   (crash_detail::Detail * detail)) {
  crash_detail::remove(detail);
}

} // namespace LIBC_NAMESPACE_DECL
