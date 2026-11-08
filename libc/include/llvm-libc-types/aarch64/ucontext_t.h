//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Definition of type ucontext_t
///
//===----------------------------------------------------------------------===//

// Note: Definitions in this file are based on the Linux kernel ABI.

#ifndef LLVM_LIBC_TYPES_AARCH64_UCONTEXT_T_H
#define LLVM_LIBC_TYPES_AARCH64_UCONTEXT_T_H

#include "../sigset_t.h"
#ifdef __ANDROID__
#include "../sigset64_t.h"
#endif
#include "../stack_t.h"
#include "mcontext_t.h"

typedef struct ucontext_t {
  // The kernel's struct ucontext. Its signal mask is padded to 1024 bits,
  // which is the size of sigset_t here, and the machine context is 16 byte
  // aligned.
  unsigned long uc_flags;
  struct ucontext_t *uc_link;
  stack_t uc_stack;
#ifdef __ANDROID__
  // bionic names the same mask a second way.
  union {
    sigset_t uc_sigmask;
    sigset64_t uc_sigmask64;
  };
#else
  sigset_t uc_sigmask;
#endif
  mcontext_t uc_mcontext;
} ucontext_t;

#endif // LLVM_LIBC_TYPES_AARCH64_UCONTEXT_T_H
