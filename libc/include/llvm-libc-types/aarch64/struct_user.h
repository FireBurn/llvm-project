//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Stands in for struct user, which aarch64 does not have.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_TYPES_AARCH64_STRUCT_USER_H
#define LLVM_LIBC_TYPES_AARCH64_STRUCT_USER_H

// The ptrace layout of struct user is x86 specific. Nothing is defined here so
// that sys/user.h can be shared by the targets that have it.

#endif // LLVM_LIBC_TYPES_AARCH64_STRUCT_USER_H
