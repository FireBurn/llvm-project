//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// A shared object whose constructor asks for the thread id and takes a lock.
/// The loader runs it before the program's own startup code has given the main
/// thread its attributes, so neither may need them.
///
//===----------------------------------------------------------------------===//

#include <pthread.h>
#include <unistd.h>

extern "C" {

int llvm_libc_dl_lock_ran = 0;
int llvm_libc_dl_lock_tid = 0;

static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

__attribute__((constructor)) static void take_the_lock() {
  llvm_libc_dl_lock_tid = static_cast<int>(gettid());
  pthread_mutex_lock(&mutex);
  llvm_libc_dl_lock_ran = 1;
  pthread_mutex_unlock(&mutex);
}
}
