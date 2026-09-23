//===-- An object that depends on the first -------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Records, in a variable the program defines, whether the object it depends
/// on was initialised by the time its own initialiser ran. See
/// init_order_test.cpp.
///
//===----------------------------------------------------------------------===//

extern "C" {

extern int llvm_libc_order_a_ready;

extern int llvm_libc_order_b_saw_a;

void llvm_libc_order_b_init() { llvm_libc_order_b_saw_a = llvm_libc_order_a_ready; }

__attribute__((section(".init_array"), used)) void (*llvm_libc_order_b_entry)() =
    llvm_libc_order_b_init;
}
