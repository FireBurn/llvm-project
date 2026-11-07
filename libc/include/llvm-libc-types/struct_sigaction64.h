//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Definition of struct sigaction64.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_TYPES_STRUCT_SIGACTION64_H
#define LLVM_LIBC_TYPES_STRUCT_SIGACTION64_H

#include "siginfo_t.h"
#include "sigset64_t.h"

// The same layout as struct sigaction, which is how bionic has it on a 64 bit
// target.
struct sigaction64 {
  union {
    void (*sa_handler)(int);
    void (*sa_sigaction)(int, siginfo_t *, void *);
  };
  sigset64_t sa_mask;
  int sa_flags;
  void (*sa_restorer)(void);
};

#endif // LLVM_LIBC_TYPES_STRUCT_SIGACTION64_H
