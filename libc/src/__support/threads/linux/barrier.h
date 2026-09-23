//===-- A platform independent abstraction layer for barriers --*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIBC___SUPPORT_SRC_THREADS_LINUX_BARRIER_H
#define LLVM_LIBC___SUPPORT_SRC_THREADS_LINUX_BARRIER_H

#include "hdr/pthread_macros.h"
#include "include/llvm-libc-types/pthread_barrier_t.h"
#include "include/llvm-libc-types/pthread_barrierattr_t.h"
#include "src/__support/threads/linux/futex_utils.h"
#include "src/__support/threads/raw_mutex.h"

namespace LIBC_NAMESPACE_DECL {

// A round ends when the last of the expected threads arrives. It starts the
// next round at once and moves the round counter on, which is what the
// others are waiting on. The threads still inside are counted so that
// destroy can wait for them to leave: the one that returns
// PTHREAD_BARRIER_SERIAL_THREAD may destroy the barrier while the others
// have yet to see the counter move.
class Barrier {
private:
  RawMutex lock;
  unsigned expected;
  unsigned arrived;
  Futex round;
  Futex inside;
  bool pshared;

public:
  static int init(Barrier *b, const pthread_barrierattr_t *attr,
                  unsigned count);
  static int destroy(Barrier *b);
  int wait();
};

static_assert(sizeof(Barrier) <= sizeof(pthread_barrier_t),
              "The public pthread_barrier_t type cannot accommodate the "
              "internal barrier type.");

static_assert(alignof(Barrier) <= alignof(pthread_barrier_t),
              "The public pthread_barrier_t type has insufficient alignment "
              "for the internal barrier type.");

} // namespace LIBC_NAMESPACE_DECL

#endif // LLVM_LIBC___SUPPORT_SRC_THREADS_LINUX_BARRIER_H
