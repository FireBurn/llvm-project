//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Tests dlvsym in a program linked against the shared libc. The object has
/// two versions of one function.
///
//===----------------------------------------------------------------------===//

#include "check.h"

#include <dlfcn.h>

using Fn = int (*)();

extern "C" int main() {
  void *handle = dlopen(LIBC_TEST_DL_OBJECT, RTLD_NOW);
  CHECK(handle != nullptr);

  Fn first = reinterpret_cast<Fn>(
      dlvsym(handle, "llvm_libc_dl_versioned", "LLVM_LIBC_DL_1"));
  Fn second = reinterpret_cast<Fn>(
      dlvsym(handle, "llvm_libc_dl_versioned", "LLVM_LIBC_DL_2"));
  CHECK(first != nullptr && second != nullptr);
  if (first && second) {
    CHECK(first() == 1);
    CHECK(second() == 2);
  }

  // Without a version, the default one answers.
  Fn plain = reinterpret_cast<Fn>(dlsym(handle, "llvm_libc_dl_versioned"));
  CHECK(plain == second);

  CHECK(dlvsym(handle, "no_such_symbol", "LLVM_LIBC_DL_1") == nullptr);
  dlerror();

  // A version the symbol is not defined under is not made up for by the
  // symbol's other one, and neither is a version the object does not have.
  CHECK(dlvsym(handle, "llvm_libc_dl_value", "LLVM_LIBC_DL_1") != nullptr);
  CHECK(dlvsym(handle, "llvm_libc_dl_value", "LLVM_LIBC_DL_2") == nullptr);
  CHECK(dlerror() != nullptr);
  CHECK(dlvsym(handle, "llvm_libc_dl_versioned", "LLVM_LIBC_DL_3") == nullptr);
  CHECK(dlerror() != nullptr);

  CHECK(dlclose(handle) == 0);
  return failures == 0 ? 0 : 1;
}
