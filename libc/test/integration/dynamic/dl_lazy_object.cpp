//===-- An object that calls through the PLT -------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Linked with lazy binding, so its calls to the program go through PLT slots
/// that start out pointing at the binder. See lazy_bind_test.cpp.
///
//===----------------------------------------------------------------------===//

extern "C" {

// Defined by the program.
double llvm_libc_lazy_target(double a, int b, double c, long d);

// Referenced and never called, so with lazy binding nothing asks for it.
void llvm_libc_lazy_missing();

double llvm_libc_lazy_call(double a) {
  // Mixed integer and floating point arguments, which the binder has to leave
  // as they were found, and a result that comes back through the same slot.
  return llvm_libc_lazy_target(a, 2, a + 1.0, 4);
}

void llvm_libc_lazy_never() { llvm_libc_lazy_missing(); }
}
