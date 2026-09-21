//===-- Test of copy relocations applied by the loader --------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Tests that a program which is not position independent sees the value of
/// a data symbol it imports from a shared object. Such a program takes a
/// copy of the data into its own .bss and the loader has to fill it in. The
/// program returns nonzero if any check fails.
///
//===----------------------------------------------------------------------===//

#include "check.h"

extern "C" {
extern int llvm_libc_dl_value;
int llvm_libc_dl_read_value();
}

extern "C" int main() {
  // The copy starts with the object's initial value.
  CHECK(llvm_libc_dl_value == 42);

  // The object's own references resolve to the program's copy, so the two
  // agree about what the variable holds.
  llvm_libc_dl_value = 7;
  CHECK(llvm_libc_dl_read_value() == 7);
  return failures == 0 ? 0 : 1;
}
