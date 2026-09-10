//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Definition of type struct user.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_TYPES_STRUCT_USER_H
#define LLVM_LIBC_TYPES_STRUCT_USER_H

#if defined(__x86_64__)
#include "x86_64/struct_user.h"
#else
#error "struct user not available for your target architecture."
#endif

#endif // LLVM_LIBC_TYPES_STRUCT_USER_H
