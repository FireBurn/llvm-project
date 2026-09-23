//===-- Test of DT_INIT and DT_FINI run by dlopen and dlclose -------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Tests that DT_INIT runs before the init array and DT_FINI after the fini
/// array, as with glibc. The program returns nonzero if any check fails.
///
//===----------------------------------------------------------------------===//

#include "check.h"

#include <dlfcn.h>

extern "C" {
// Exported by the program for the object to write into, since the object is
// unmapped before its finalisers can be looked at.
int llvm_libc_host_fini_count;
int llvm_libc_host_fini_order[4];
}

extern "C" int main() {
  void *handle = dlopen(LIBC_TEST_DL_INIT_OBJECT, RTLD_NOW);
  CHECK(handle != nullptr);
  if (!handle)
    return 1;

  int *count = static_cast<int *>(dlsym(handle, "llvm_libc_dl_init_count"));
  int *order = static_cast<int *>(dlsym(handle, "llvm_libc_dl_init_order"));
  CHECK(count != nullptr && order != nullptr);
  if (count && order) {
    CHECK(*count == 2);
    // DT_INIT first, then the init array.
    CHECK(order[0] == 1);
    CHECK(order[1] == 2);
  }

  CHECK(llvm_libc_host_fini_count == 0);
  CHECK(dlclose(handle) == 0);
  CHECK(llvm_libc_host_fini_count == 2);
  // The fini array first, then DT_FINI.
  CHECK(llvm_libc_host_fini_order[0] == 3);
  CHECK(llvm_libc_host_fini_order[1] == 4);
  return failures == 0 ? 0 : 1;
}
