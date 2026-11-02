//===-- Test of lazy binding of PLT slots --------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Tests that an object opened with RTLD_LAZY has its calls bound when they
/// are first made, with the arguments and the result intact, and that a call
/// it never makes need not be resolvable. Opened with RTLD_NOW the same object
/// is refused. The program returns nonzero if any check fails.
///
//===----------------------------------------------------------------------===//

#include "check.h"

#include <dlfcn.h>

extern "C" double llvm_libc_lazy_target(double a, int b, double c, long d) {
  return a + b * 10 + c * 100 + d * 1000;
}

extern "C" int main() {
  // The call that cannot be resolved is made by nothing, so it is only
  // refused when the whole object is bound at once.
  CHECK(dlopen(LIBC_TEST_DL_LAZY_OBJECT, RTLD_NOW) == nullptr);
  CHECK(dlerror() != nullptr);

  void *handle = dlopen(LIBC_TEST_DL_LAZY_OBJECT, RTLD_LAZY);
  CHECK(handle != nullptr);
  if (!handle)
    return 1;
  auto call = reinterpret_cast<double (*)(double)>(
      dlsym(handle, "llvm_libc_lazy_call"));
  CHECK(call != nullptr);
  if (!call)
    return 1;

  // The first call binds the slot, and the second uses it.
  const double expected = 1.5 + 20 + 2.5 * 100 + 4000;
  CHECK(call(1.5) == expected);
  CHECK(call(1.5) == expected);
  CHECK(call(0.0) == 0 + 20 + 1.0 * 100 + 4000);
  CHECK(dlclose(handle) == 0);
  return failures == 0 ? 0 : 1;
}
