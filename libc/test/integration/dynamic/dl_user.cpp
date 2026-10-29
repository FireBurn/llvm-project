//===-- Shared object that uses a symbol it does not define ---------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Calls a function of the test object without being linked against it, so
/// whether it loads depends on that object being in the global scope.
///
//===----------------------------------------------------------------------===//

extern "C" {

int llvm_libc_dl_add(int a, int b);

int llvm_libc_dl_user() { return llvm_libc_dl_add(1, 2); }
}
