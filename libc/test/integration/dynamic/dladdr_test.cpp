//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Tests dladdr in a program linked against the shared libc.
///
//===----------------------------------------------------------------------===//

#include "check.h"

#include <dlfcn.h>
#include <string.h>

static int in_the_program() { return 1; }

extern "C" int main() {
  void *handle = dlopen(LIBC_TEST_DL_OBJECT, RTLD_NOW);
  CHECK(handle != nullptr);
  void *add = dlsym(handle, "llvm_libc_dl_add");
  CHECK(add != nullptr);

  Dl_info info;
  memset(&info, 0, sizeof(info));
  CHECK(dladdr(add, &info) != 0);
  CHECK(info.dli_fname != nullptr &&
        strcmp(info.dli_fname, LIBC_TEST_DL_OBJECT) == 0);
  CHECK(info.dli_fbase != nullptr);
  CHECK(info.dli_sname != nullptr &&
        strcmp(info.dli_sname, "llvm_libc_dl_add") == 0);
  CHECK(info.dli_saddr == add);

  // An address inside a function, not at its start, still names it.
  memset(&info, 0, sizeof(info));
  CHECK(dladdr(static_cast<char *>(add) + 1, &info) != 0);
  CHECK(info.dli_sname != nullptr &&
        strcmp(info.dli_sname, "llvm_libc_dl_add") == 0);
  CHECK(info.dli_saddr == add);

  // The program itself is a module too, and so is the library.
  memset(&info, 0, sizeof(info));
  CHECK(dladdr(reinterpret_cast<void *>(in_the_program), &info) != 0);
  CHECK(info.dli_fname != nullptr);
  CHECK(dladdr(reinterpret_cast<void *>(dlopen), &info) != 0);
  CHECK(info.dli_sname != nullptr && strcmp(info.dli_sname, "dlopen") == 0);

  // No module holds address zero.
  CHECK(dladdr(nullptr, &info) == 0);

  CHECK(dlclose(handle) == 0);
  // Once closed the object's addresses belong to nothing.
  CHECK(dladdr(add, &info) == 0);
  return failures == 0 ? 0 : 1;
}
