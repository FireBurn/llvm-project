//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Definition of sigset64_t.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_TYPES_SIGSET64_T_H
#define LLVM_LIBC_TYPES_SIGSET64_T_H

#include "sigset_t.h"

// sigset_t is already large enough for every signal on a 64 bit target, which
// is how bionic defines the 64 bit form there.
typedef sigset_t sigset64_t;

#endif // LLVM_LIBC_TYPES_SIGSET64_T_H
