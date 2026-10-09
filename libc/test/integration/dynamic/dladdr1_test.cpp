//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Tests dladdr1 in a program linked against the shared libc.
///
//===----------------------------------------------------------------------===//

#include "check.h"

#include <dlfcn.h>
#include <elf.h>
#include <link.h>
#include <string.h>

extern "C" int main() {
  void *handle = dlopen(LIBC_TEST_DL_OBJECT, RTLD_NOW);
  CHECK(handle != nullptr);
  void *add = dlsym(handle, "llvm_libc_dl_add");
  CHECK(add != nullptr);

  Dl_info info;
  void *found = nullptr;
  CHECK(dladdr1(add, &info, &found,
                RTLD_DL_SYMENT) != 0);
  const ElfW(Sym) *symbol = static_cast<const ElfW(Sym) *>(found);
  CHECK(symbol != nullptr);
  if (symbol) {
    CHECK(ELF64_ST_TYPE(symbol->st_info) == STT_FUNC);
    CHECK(info.dli_sname != nullptr &&
          strcmp(info.dli_sname, "llvm_libc_dl_add") == 0);
  }

  struct link_map *map = nullptr;
  CHECK(dladdr1(add, &info, reinterpret_cast<void **>(&map), RTLD_DL_LINKMAP) !=
        0);
  CHECK(map != nullptr);
  if (map)
    CHECK(map->l_name != nullptr &&
          strcmp(map->l_name, LIBC_TEST_DL_OBJECT) == 0);

  // A kind of extra information it does not know is left unfilled, as in
  // glibc, and the lookup itself still answers.
  void *extra = nullptr;
  CHECK(dladdr1(add, &info, &extra, 99) != 0);
  CHECK(extra == nullptr);

  CHECK(dlclose(handle) == 0);
  return failures == 0 ? 0 : 1;
}
