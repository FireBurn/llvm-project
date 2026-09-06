//===-- Unittests for pthread_kill and pthread_getattr_np -----------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "hdr/errno_macros.h"
#include "hdr/signal_macros.h"
#include "hdr/stdint_proxy.h"
#include "src/pthread/pthread_attr_destroy.h"
#include "src/pthread/pthread_attr_getstack.h"
#include "src/pthread/pthread_getattr_np.h"
#include "src/pthread/pthread_kill.h"
#include "src/pthread/pthread_self.h"
#include "src/signal/signal.h"
#include "test/UnitTest/Test.h"

#include <pthread.h>

namespace {

volatile int delivered = 0;

void handler(int) { delivered = 1; }

} // anonymous namespace

// A signal sent to a thread is delivered to that thread. Sending it to the
// calling one is the only case a test without a second thread can make.
TEST(LlvmLibcPthreadKillTest, SignalsAThread) {
  ASSERT_NE(LIBC_NAMESPACE::signal(SIGUSR1, &handler), SIG_ERR);
  delivered = 0;

  ASSERT_EQ(
      LIBC_NAMESPACE::pthread_kill(LIBC_NAMESPACE::pthread_self(), SIGUSR1), 0);
  ASSERT_EQ(delivered, 1);
}

// A signal of zero asks whether the thread is there rather than sending
// anything, and a number that is not a signal is refused.
TEST(LlvmLibcPthreadKillTest, AskingWhetherAThreadIsThere) {
  ASSERT_EQ(LIBC_NAMESPACE::pthread_kill(LIBC_NAMESPACE::pthread_self(), 0), 0);
  ASSERT_EQ(LIBC_NAMESPACE::pthread_kill(LIBC_NAMESPACE::pthread_self(), -1),
            EINVAL);
}

// pthread_getattr_np reports the attributes a running thread actually has,
// rather than the ones it was asked for, so the stack it names is the one
// the thread is on.
TEST(LlvmLibcPthreadGetattrNpTest, ReportsTheStackTheThreadIsOn) {
  pthread_attr_t attr;
  ASSERT_EQ(
      LIBC_NAMESPACE::pthread_getattr_np(LIBC_NAMESPACE::pthread_self(), &attr),
      0);

  void *stack = nullptr;
  size_t size = 0;
  ASSERT_EQ(LIBC_NAMESPACE::pthread_attr_getstack(&attr, &stack, &size), 0);
  ASSERT_FALSE(stack == nullptr);
  ASSERT_GT(size, size_t(0));

  // A local of this function is somewhere in the range it named.
  char here = 0;
  auto address = reinterpret_cast<uintptr_t>(&here);
  auto low = reinterpret_cast<uintptr_t>(stack);
  ASSERT_GE(address, low);
  ASSERT_LT(address, low + size);

  ASSERT_EQ(LIBC_NAMESPACE::pthread_attr_destroy(&attr), 0);
}
