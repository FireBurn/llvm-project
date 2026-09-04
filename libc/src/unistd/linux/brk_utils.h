//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// The program break, which brk and sbrk share.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC_UNISTD_LINUX_BRK_UTILS_H
#define LLVM_LIBC_SRC_UNISTD_LINUX_BRK_UTILS_H

#include "src/__support/macros/attributes.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {
namespace internal {

// Where the break is now. sbrk has to report the old break as well as move
// it, and the syscall only reports the new one, so it is remembered here.
// A null value means it has not been asked for yet.
extern void *current_break;

} // namespace internal
} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC_UNISTD_LINUX_BRK_UTILS_H
