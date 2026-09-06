//===-- Test of DT_INIT and DT_FINI run by dlopen and dlclose -------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Tests that an initialiser of a library the program needs can ask for the
/// thread id and take a mutex, which the loader runs before the main thread
/// has its attributes. The
/// program returns nonzero if any check fails.
///
//===----------------------------------------------------------------------===//

#include "check.h"

#include <unistd.h>

extern "C" int llvm_libc_dl_lock_ran;
extern "C" int llvm_libc_dl_lock_tid;

extern "C" int main() {
  CHECK(llvm_libc_dl_lock_ran == 1);
  // The main thread's id is the process id.
  CHECK(llvm_libc_dl_lock_tid == getpid());
  return failures == 0 ? 0 : 1;
}
