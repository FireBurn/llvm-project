//===-- The object the others depend on ----------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Records that its initialiser has run. See init_order_test.cpp.
///
//===----------------------------------------------------------------------===//

extern "C" {

int llvm_libc_order_a_ready;

void llvm_libc_order_a_init() { llvm_libc_order_a_ready = 1; }

// Placed in the array by hand, so that it is there whatever the compiler is
// configured to do with constructors.
__attribute__((section(".init_array"), used)) void (*llvm_libc_order_a_entry)() =
    llvm_libc_order_a_init;
}
