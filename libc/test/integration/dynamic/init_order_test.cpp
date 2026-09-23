//===-- Test of the order shared objects are initialised in --------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Tests that an object is initialised after the ones it depends on, as with
/// glibc. The program loads the object another depends on first, so the
/// order objects are loaded in, which is breadth first, puts the
/// dependent one later and running that backwards would initialise it first.
/// The dependent object is reached both through the program's own list of
/// libraries and through dlopen of an object that names both.
/// The program returns nonzero if any check fails.
///
//===----------------------------------------------------------------------===//

#include "check.h"

#ifdef LIBC_TEST_DLOPEN
#include <dlfcn.h>
#endif

// Where the dependent object's initialiser leaves what it saw of the other.
// The program exports it, so that the object can reach it.
extern "C" int llvm_libc_order_b_saw_a = -1;

extern "C" int main() {
#ifdef LIBC_TEST_DLOPEN
  void *handle = dlopen(LIBC_TEST_DL_ORDER_OBJECT, RTLD_NOW);
  CHECK(handle != nullptr);
  if (!handle)
    return 1;
#endif
  CHECK(llvm_libc_order_b_saw_a == 1);
  return failures == 0 ? 0 : 1;
}
