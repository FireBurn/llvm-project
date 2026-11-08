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
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
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

// A link to an object that is open, or a second name for it, is that object:
// a copy would have its own state, and RTLD_NOLOAD would not find it.
static void another_name_for_an_open_object_is_the_same_object() {
  char dir[64];
  snprintf(dir, sizeof(dir), "/tmp/llvm_libc_dl_alias_%d",
           static_cast<int>(getpid()));
  CHECK(mkdir(dir, 0700) == 0);
  char soft[96];
  char hard[96];
  snprintf(soft, sizeof(soft), "%s/llvm_libc_dl_soft.so", dir);
  snprintf(hard, sizeof(hard), "%s/llvm_libc_dl_hard.so", dir);
  CHECK(symlink(LIBC_TEST_DL_OBJECT, soft) == 0);
  const bool linked = link(LIBC_TEST_DL_OBJECT, hard) == 0;

  void *handle = dlopen(LIBC_TEST_DL_OBJECT, RTLD_NOW);
  CHECK(handle != nullptr);
  int opened = 1;

  // A path that ends in a different name.
  // RTLD_NOLOAD that finds the object counts as an open, as it does for the
  // name the object was opened under.
  CHECK(dlopen(soft, RTLD_NOW | RTLD_NOLOAD) == handle);
  CHECK(dlopen(soft, RTLD_NOW) == handle);
  opened += 2;
  if (linked) {
    CHECK(dlopen(hard, RTLD_NOW) == handle);
    ++opened;
  }

  // A bare name found through the search path.
  const char *saved = getenv("LD_LIBRARY_PATH");
  CHECK(setenv("LD_LIBRARY_PATH", dir, 1) == 0);
  CHECK(dlopen("llvm_libc_dl_soft.so", RTLD_NOW | RTLD_NOLOAD) == handle);
  CHECK(dlopen("llvm_libc_dl_soft.so", RTLD_NOW) == handle);
  opened += 2;
  if (saved)
    setenv("LD_LIBRARY_PATH", saved, 1);
  else
    unsetenv("LD_LIBRARY_PATH");

  // One object, so one copy of its data.
  int *value = static_cast<int *>(dlsym(handle, "llvm_libc_dl_value"));
  void *again = dlopen(soft, RTLD_NOW);
  ++opened;
  int *through_link = static_cast<int *>(dlsym(again, "llvm_libc_dl_value"));
  CHECK(value != nullptr && value == through_link);

  // Each open has to be closed, and only the last one unloads it.
  for (int i = 0; i < opened; ++i)
    CHECK(dlclose(handle) == 0);
  CHECK(dlopen(soft, RTLD_NOW | RTLD_NOLOAD) == nullptr);
  dlerror();

  unlink(soft);
  if (linked)
    unlink(hard);
  rmdir(dir);
}

static void the_main_program_has_a_handle() {
  void *self = dlopen(nullptr, RTLD_NOW);
  CHECK(self != nullptr);
  CHECK(dlsym(self, "dlopen") != nullptr);
  CHECK(dlclose(self) == 0);
}

// More objects than the block the module set starts with, each a copy of the
// test object so that none of them is taken for one already open.
static void more_objects_than_the_initial_block_can_be_open() {
  constexpr int COUNT = 80;
  static void *handles[COUNT];
  static char paths[COUNT][64];

  int source = open(LIBC_TEST_DL_OBJECT, O_RDONLY);
  CHECK(source >= 0);
  if (source < 0)
    return;
  static char image[1 << 16];
  long size = read(source, image, sizeof(image));
  close(source);
  CHECK(size > 0 && size < static_cast<long>(sizeof(image)));
  if (size <= 0)
    return;

  int opened = 0;
  for (int i = 0; i < COUNT; ++i) {
    snprintf(paths[i], sizeof(paths[i]), "/tmp/llvm_libc_dl_grow_%d_%d.so",
             static_cast<int>(getpid()), i);
    int out = open(paths[i], O_WRONLY | O_CREAT | O_TRUNC, 0700);
    CHECK(out >= 0);
    if (out < 0)
      break;
    CHECK(write(out, image, size) == size);
    close(out);
    handles[i] = dlopen(paths[i], RTLD_NOW);
    CHECK(handles[i] != nullptr);
    if (handles[i])
      ++opened;
  }
  CHECK(opened == COUNT);

  // The ones that came after the initial block still resolve their symbols.
  for (int i = 0; i < COUNT; ++i) {
    if (!handles[i])
      continue;
    using AddFn = int (*)(int, int);
    AddFn add = reinterpret_cast<AddFn>(dlsym(handles[i], "llvm_libc_dl_add"));
    CHECK(add != nullptr);
    if (add)
      CHECK(add(i, 1) == i + 1);
  }
  for (int i = COUNT - 1; i >= 0; --i) {
    if (handles[i])
      CHECK(dlclose(handles[i]) == 0);
    unlink(paths[i]);
  }
}

extern "C" int main() {
  loads_an_object_and_finds_its_symbols();
  an_unknown_symbol_is_an_error();
  a_missing_file_is_an_error();
  opening_twice_gives_the_same_handle();
  noload_needs_an_object_that_is_loaded();
  another_name_for_an_open_object_is_the_same_object();
  the_main_program_has_a_handle();
  more_objects_than_the_initial_block_can_be_open();
  return failures == 0 ? 0 : 1;
}
