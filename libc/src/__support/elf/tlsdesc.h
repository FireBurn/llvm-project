//===-- TLS descriptor resolvers --------------------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC_SRC___SUPPORT_ELF_TLSDESC_H
#define LLVM_LIBC_SRC___SUPPORT_ELF_TLSDESC_H

#include "src/__support/macros/properties/architectures.h"

#if defined(LIBC_TARGET_ARCH_IS_X86_64)

// A TLS descriptor is a function and an argument. Code reaching a thread
// local through one loads the descriptor's address into %rax, calls the
// function, and adds what comes back in %rax to the thread pointer. The
// function may change nothing else.
//
// For a module that has a place in every thread's static block, the argument
// is already that offset, and this hands it back. It is emitted in a COMDAT
// group so that the loader and libc, which both bind modules, each carry one
// copy however many of their sources include this.
asm(R"(
  .pushsection .text.__llvm_libc_tlsdesc_static,"axG",@progbits,__llvm_libc_tlsdesc_static,comdat
  .p2align 4
  .weak __llvm_libc_tlsdesc_static
  .hidden __llvm_libc_tlsdesc_static
  .type __llvm_libc_tlsdesc_static, @function
__llvm_libc_tlsdesc_static:
  endbr64
  movq 8(%rax), %rax
  ret
  .size __llvm_libc_tlsdesc_static, . - __llvm_libc_tlsdesc_static
  .popsection
)");

extern "C" void __llvm_libc_tlsdesc_static();

#endif

#endif // LLVM_LIBC_SRC___SUPPORT_ELF_TLSDESC_H
