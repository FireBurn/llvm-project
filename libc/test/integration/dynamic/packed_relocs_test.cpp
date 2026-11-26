//===-- Test of packed relocations
//------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
/// \file
/// The loader applies relocations packed in Android's form.
///
//===----------------------------------------------------------------------===//

#include "check.h"

extern "C" {
int llvm_libc_packed_values[4] = {10, 20, 30, 40};
int llvm_libc_packed_sum();
}

extern "C" int main() {
  CHECK(llvm_libc_packed_sum() == 110);
  return failures == 0 ? 0 : 1;
}
