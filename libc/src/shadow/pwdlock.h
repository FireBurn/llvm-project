//===-- The lock lckpwdf and ulckpwdf share ---------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC_SHADOW_PWDLOCK_H
#define LLVM_LIBC_SRC_SHADOW_PWDLOCK_H

#include "src/__support/macros/attributes.h"
#include "src/__support/macros/config.h"

namespace LIBC_NAMESPACE_DECL {
namespace shadow_lock {

// The file everything which edits the user databases agrees to lock, and
// the descriptor the lock is held on. It is -1 when nothing is held.
LIBC_INLINE_VAR constexpr const char *LOCK_PATH = "/etc/.pwd.lock";

extern int held_fd;

} // namespace shadow_lock
} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC_SRC_SHADOW_PWDLOCK_H
