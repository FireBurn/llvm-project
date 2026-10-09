//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Tests dlinfo in a program linked against the shared libc.
///
//===----------------------------------------------------------------------===//

#include "check.h"

#include <dlfcn.h>
#include <link.h>
#include <string.h>

extern "C" int main() {
  void *handle = dlopen(LIBC_TEST_DL_OBJECT, RTLD_NOW);
  CHECK(handle != nullptr);

  struct link_map *map = nullptr;
  CHECK(dlinfo(handle, RTLD_DI_LINKMAP, &map) == 0);
  CHECK(map != nullptr);
  if (map) {
    CHECK(map->l_name != nullptr &&
          strcmp(map->l_name, LIBC_TEST_DL_OBJECT) == 0);
    CHECK(map->l_addr != 0);
    CHECK(map->l_ld != nullptr);
  }

  Lmid_t lmid = LM_ID_NEWLM;
  CHECK(dlinfo(handle, RTLD_DI_LMID, &lmid) == 0);
  CHECK(lmid == LM_ID_BASE);

  char origin[4096];
  CHECK(dlinfo(handle, RTLD_DI_ORIGIN, origin) == 0);
  // The directory the object was loaded from.
  const char *last = strrchr(LIBC_TEST_DL_OBJECT, '/');
  CHECK(last != nullptr &&
        strlen(origin) == static_cast<size_t>(last - LIBC_TEST_DL_OBJECT));
  CHECK(strncmp(origin, LIBC_TEST_DL_OBJECT, strlen(origin)) == 0);

  // A request it does not know is refused.
  CHECK(dlinfo(handle, -1, &lmid) == -1);
  CHECK(dlerror() != nullptr);

  CHECK(dlclose(handle) == 0);
  return failures == 0 ? 0 : 1;
}
