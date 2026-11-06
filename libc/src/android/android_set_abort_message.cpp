//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation of android_set_abort_message.
///
//===----------------------------------------------------------------------===//

#include "src/android/android_set_abort_message.h"
#include "src/__support/abort_message.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {

LLVM_LIBC_FUNCTION(void, android_set_abort_message, (const char *msg)) {
  if (msg == nullptr)
    return;
  abort_message::set(msg);
}

} // namespace LIBC_NAMESPACE_DECL
