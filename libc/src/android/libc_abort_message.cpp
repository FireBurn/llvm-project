//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation of __libc_abort_message.
///
//===----------------------------------------------------------------------===//

#include "src/android/libc_abort_message.h"
#include "src/__support/abort_message.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {

// The block is what debuggerd reads as the abort message of a process, from
// debugger_process_info. It is null until a message has been set, as it is
// in bionic.
LLVM_LIBC_FUNCTION(void *, __libc_abort_message, ()) {
  if (abort_message::get() == nullptr)
    return nullptr;
  return &abort_message::block();
}

} // namespace LIBC_NAMESPACE_DECL
