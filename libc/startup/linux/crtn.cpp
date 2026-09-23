//===-- Epilogues of _init and _fini --------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// See crti.cpp.

#if defined(__x86_64__)
asm(R"(
  .section .init,"ax",@progbits
  pop %rax
  ret

  .section .fini,"ax",@progbits
  pop %rax
  ret
)");
#elif defined(__aarch64__)
asm(R"(
  .section .init,"ax",%progbits
  ldp x29, x30, [sp], 16
  ret

  .section .fini,"ax",%progbits
  ldp x29, x30, [sp], 16
  ret
)");
#endif
