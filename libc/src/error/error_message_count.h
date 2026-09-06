//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation header for error_message_count.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC_ERROR_ERROR_MESSAGE_COUNT_H
#define LLVM_LIBC_SRC_ERROR_ERROR_MESSAGE_COUNT_H

#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {

extern LIBC_SHARED_INTERNAL unsigned int error_message_count;

} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC_ERROR_ERROR_MESSAGE_COUNT_H
