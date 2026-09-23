//===-- Prologues of _init and _fini --------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// _init and _fini are what DT_INIT and DT_FINI point at. The .init and .fini
// sections of every object linked between crti.o and crtn.o land between
// the prologue here and the epilogue in crtn.o, as with glibc and musl. Little
// uses them now that init_array has taken their place, but some tools that
// rewrite an object after it is linked, firefox's relrhack among them, run
// their own code through DT_INIT.

#if defined(__x86_64__)
asm(R"(
  .section .init,"ax",@progbits
  .globl _init
  .hidden _init
  .type _init,@function
_init:
  push %rax

  .section .fini,"ax",@progbits
  .globl _fini
  .hidden _fini
  .type _fini,@function
_fini:
  push %rax
)");
#elif defined(__aarch64__)
asm(R"(
  .section .init,"ax",%progbits
  .globl _init
  .hidden _init
  .type _init,%function
_init:
  stp x29, x30, [sp, -16]!
  mov x29, sp

  .section .fini,"ax",%progbits
  .globl _fini
  .hidden _fini
  .type _fini,%function
_fini:
  stp x29, x30, [sp, -16]!
  mov x29, sp
)");
#endif
