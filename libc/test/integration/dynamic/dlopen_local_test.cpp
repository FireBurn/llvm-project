//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Tests that RTLD_LOCAL keeps an object out of the global scope, for lookups
/// and for the objects opened afterwards, and that RTLD_GLOBAL puts it back.
///
//===----------------------------------------------------------------------===//

#include "check.h"

#include <dlfcn.h>

using Fn = int (*)();

extern "C" int main() {
  // Opened locally, the object is reachable through its handle only.
  void *local = dlopen(LIBC_TEST_DL_OBJECT, RTLD_NOW | RTLD_LOCAL);
  CHECK(local != nullptr);
  CHECK(dlsym(local, "llvm_libc_dl_add") != nullptr);
  CHECK(dlsym(RTLD_DEFAULT, "llvm_libc_dl_add") == nullptr);
  CHECK(dlerror() != nullptr);

  // An object which uses it without being linked against it cannot be opened.
  CHECK(dlopen(LIBC_TEST_DL_USER, RTLD_NOW) == nullptr);
  CHECK(dlerror() != nullptr);

  // Opening it again, now globally, puts it in the global scope.
  void *global = dlopen(LIBC_TEST_DL_OBJECT, RTLD_NOW | RTLD_GLOBAL);
  CHECK(global != nullptr);
  CHECK(dlsym(RTLD_DEFAULT, "llvm_libc_dl_add") ==
        dlsym(local, "llvm_libc_dl_add"));

  void *user = dlopen(LIBC_TEST_DL_USER, RTLD_NOW);
  CHECK(user != nullptr);
  if (user) {
    Fn call = reinterpret_cast<Fn>(dlsym(user, "llvm_libc_dl_user"));
    CHECK(call != nullptr);
    if (call)
      CHECK(call() == 3);
    CHECK(dlclose(user) == 0);
  }
  CHECK(dlclose(global) == 0);
  CHECK(dlclose(local) == 0);

  // The default is local.
  void *plain = dlopen(LIBC_TEST_DL_OBJECT, RTLD_NOW);
  CHECK(plain != nullptr);
  CHECK(dlsym(RTLD_DEFAULT, "llvm_libc_dl_add") == nullptr);
  CHECK(dlclose(plain) == 0);

  return failures == 0 ? 0 : 1;
}
