//===-- Object that reads the tag mask in its initialiser -----------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Records the memory tagging control its initialiser sees. See
/// init_mte_test.cpp.
///
//===----------------------------------------------------------------------===//

extern "C" {

// Defined by the program, which exports it.
extern long llvm_libc_mte_ctrl_in_init;

void llvm_libc_mte_init() {
#if defined(__aarch64__)
  // PR_GET_TAGGED_ADDR_CTRL through the syscall, as this object has no libc
  // of its own to call.
  register long x0 __asm__("x0") = 56;
  register long x1 __asm__("x1") = 0;
  register long x2 __asm__("x2") = 0;
  register long x3 __asm__("x3") = 0;
  register long x4 __asm__("x4") = 0;
  register long x8 __asm__("x8") = 167;
  __asm__ volatile("svc #0"
                   : "+r"(x0)
                   : "r"(x1), "r"(x2), "r"(x3), "r"(x4), "r"(x8)
                   : "memory");
  llvm_libc_mte_ctrl_in_init = x0;
#endif
}

// Placed in the array by hand, so that it is there whatever the compiler is
// configured to do with constructors.
__attribute__((section(".init_array"), used)) void (*llvm_libc_mte_entry)() =
    llvm_libc_mte_init;
}
