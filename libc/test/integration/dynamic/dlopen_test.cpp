//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Tests dlopen, dlsym, dlclose and dlerror in a program linked against the
/// shared libc and started by its loader, the only way they have anything to
/// work on. The program returns nonzero if any check fails.
///
//===----------------------------------------------------------------------===//

#include "check.h"

#include <dlfcn.h>
using AddFn = int (*)(int, int);
using ReadFn = int (*)();

static void loads_an_object_and_finds_its_symbols() {
  void *handle = dlopen(LIBC_TEST_DL_OBJECT, RTLD_NOW);
  CHECK(handle != nullptr);
  if (!handle)
    return;

  AddFn add = reinterpret_cast<AddFn>(dlsym(handle, "llvm_libc_dl_add"));
  CHECK(add != nullptr);
  if (add)
    CHECK(add(2, 3) == 5);

  int *value = static_cast<int *>(dlsym(handle, "llvm_libc_dl_value"));
  CHECK(value != nullptr);
  ReadFn read =
      reinterpret_cast<ReadFn>(dlsym(handle, "llvm_libc_dl_read_value"));
  CHECK(read != nullptr);
  if (value && read) {
    CHECK(*value == 42);
    // The object's own reference to its data lands on the same variable.
    *value = 7;
    CHECK(read() == 7);
  }
  CHECK(dlclose(handle) == 0);
}

static void an_unknown_symbol_is_an_error() {
  void *handle = dlopen(LIBC_TEST_DL_OBJECT, RTLD_LAZY);
  CHECK(handle != nullptr);
  CHECK(dlerror() == nullptr);
  CHECK(dlsym(handle, "no_such_symbol") == nullptr);
  const char *message = dlerror();
  CHECK(message != nullptr && message[0] != '\0');
  // dlerror reports an error once.
  CHECK(dlerror() == nullptr);
  CHECK(dlclose(handle) == 0);
}

static void a_missing_file_is_an_error() {
  CHECK(dlopen("/nonexistent/libnothing.so", RTLD_NOW) == nullptr);
  const char *message = dlerror();
  CHECK(message != nullptr && message[0] != '\0');
  CHECK(dlerror() == nullptr);
  CHECK(dlopen("/etc/hostname", RTLD_NOW) == nullptr);
  CHECK(dlerror() != nullptr);
}

static void opening_twice_gives_the_same_handle() {
  void *first = dlopen(LIBC_TEST_DL_OBJECT, RTLD_NOW);
  void *second = dlopen(LIBC_TEST_DL_OBJECT, RTLD_NOW);
  CHECK(first != nullptr);
  CHECK(first == second);
  CHECK(dlclose(first) == 0);
  // Still open once, so its symbols are still there.
  CHECK(dlsym(second, "llvm_libc_dl_add") != nullptr);
  CHECK(dlclose(second) == 0);
}

static void noload_needs_an_object_that_is_loaded() {
  CHECK(dlopen(LIBC_TEST_DL_OBJECT, RTLD_NOW | RTLD_NOLOAD) == nullptr);
  dlerror();
  void *handle = dlopen(LIBC_TEST_DL_OBJECT, RTLD_NOW);
  CHECK(handle != nullptr);
  CHECK(dlopen(LIBC_TEST_DL_OBJECT, RTLD_NOW | RTLD_NOLOAD) == handle);
  CHECK(dlclose(handle) == 0);
  CHECK(dlclose(handle) == 0);
}

static void the_main_program_has_a_handle() {
  void *self = dlopen(nullptr, RTLD_NOW);
  CHECK(self != nullptr);
  CHECK(dlsym(self, "dlopen") != nullptr);
  CHECK(dlclose(self) == 0);
}

extern "C" int main() {
  loads_an_object_and_finds_its_symbols();
  an_unknown_symbol_is_an_error();
  a_missing_file_is_an_error();
  opening_twice_gives_the_same_handle();
  noload_needs_an_object_that_is_loaded();
  the_main_program_has_a_handle();
  return failures == 0 ? 0 : 1;
}
