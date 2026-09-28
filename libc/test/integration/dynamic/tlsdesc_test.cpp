//===-- Test of TLS descriptor relocations --------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Tests that a thread local reached through a TLS descriptor works, both in
/// an object loaded at startup, which has a place in the static block, and in
/// one opened later with dlopen. Each thread has a value of its own. The
/// program returns nonzero if any check fails.
///
//===----------------------------------------------------------------------===//

#include "check.h"

#include <pthread.h>

#ifdef LIBC_TEST_DLOPEN
#include <dlfcn.h>
#else
extern "C" int *llvm_libc_tlsdesc_address();
#endif

static int *(*address)();

static void *other_thread(void *) {
  int *mine = address();
  return mine == nullptr ? nullptr
                         : reinterpret_cast<void *>(
                               static_cast<long>(*mine) | (1L << 32));
}

extern "C" int main() {
#ifdef LIBC_TEST_DLOPEN
  void *handle = dlopen(LIBC_TEST_DL_TLSDESC_OBJECT, RTLD_NOW);
  CHECK(handle != nullptr);
  if (!handle)
    return 1;
  address = reinterpret_cast<int *(*)()>(
      dlsym(handle, "llvm_libc_tlsdesc_address"));
  CHECK(address != nullptr);
  if (!address)
    return 1;
#else
  address = llvm_libc_tlsdesc_address;
#endif

  int *value = address();
  CHECK(value != nullptr);
  if (!value)
    return 1;
  CHECK(*value == 7);
  *value = 9;
  CHECK(*address() == 9);

  // A new thread starts from the initial value, not from this one's.
  pthread_t thread;
  CHECK(pthread_create(&thread, nullptr, other_thread, nullptr) == 0);
  void *result = nullptr;
  CHECK(pthread_join(thread, &result) == 0);
  CHECK(result == reinterpret_cast<void *>(7L | (1L << 32)));
  CHECK(*address() == 9);
  return failures == 0 ? 0 : 1;
}
