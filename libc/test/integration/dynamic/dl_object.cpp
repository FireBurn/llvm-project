//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// A shared object for the dlfcn tests to load. It needs no library.
///
//===----------------------------------------------------------------------===//

extern "C" {

int llvm_libc_dl_value = 42;

int llvm_libc_dl_add(int a, int b) { return a + b; }

// Reads its own data through the module's relocations, which only come out
// right once the loader has applied them.
int llvm_libc_dl_read_value() { return llvm_libc_dl_value; }
}
