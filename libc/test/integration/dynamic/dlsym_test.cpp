//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Tests dlsym's RTLD_DEFAULT and RTLD_NEXT in a program linked against the
/// shared libc.
///
//===----------------------------------------------------------------------===//

#include "check.h"

#include <dlfcn.h>

extern "C" int main() {
  // Everything the program is linked to is in the global scope.
  CHECK(dlsym(RTLD_DEFAULT, "dlopen") != nullptr);
  CHECK(dlsym(RTLD_DEFAULT, "no_such_symbol") == nullptr);
  CHECK(dlerror() != nullptr);

  // An object opened globally is.
  void *global = dlopen(LIBC_TEST_DL_OBJECT, RTLD_NOW | RTLD_GLOBAL);
  CHECK(global != nullptr);
  CHECK(dlsym(RTLD_DEFAULT, "llvm_libc_dl_add") ==
        dlsym(global, "llvm_libc_dl_add"));
  CHECK(dlclose(global) == 0);

  // The handle for the program reaches the same things.
  void *self = dlopen(nullptr, RTLD_NOW);
  CHECK(self != nullptr);
  CHECK(dlsym(self, "dlopen") == dlsym(RTLD_DEFAULT, "dlopen"));
  CHECK(dlclose(self) == 0);

  // The next definition after the program's own is the library's, and a
  // name nothing else defines has none.
  CHECK(dlsym(RTLD_NEXT, "dlopen") != nullptr);
  CHECK(dlsym(RTLD_NEXT, "no_such_symbol") == nullptr);
  dlerror();
  return failures == 0 ? 0 : 1;
}
