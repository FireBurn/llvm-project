//===-- Unittests for siginterrupt ----------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "hdr/signal_macros.h"
#include "hdr/types/struct_sigaction.h"
#include "src/__support/libc_errno.h"
#include "src/signal/sigaction.h"
#include "src/signal/sigemptyset.h"
#include "src/signal/siginterrupt.h"
#include "test/UnitTest/ErrnoCheckingTest.h"
#include "test/UnitTest/Test.h"

using LlvmLibcSigInterruptTest = LIBC_NAMESPACE::testing::ErrnoCheckingTest;

namespace {
void handler(int) {}
} // anonymous namespace

// Asking for a signal to interrupt a slow call clears SA_RESTART, and asking
// for the call to be started again sets it.
TEST_F(LlvmLibcSigInterruptTest, TurnsRestartOnAndOff) {
  struct sigaction action;
  action.sa_handler = &handler;
  action.sa_flags = SA_RESTART;
  LIBC_NAMESPACE::sigemptyset(&action.sa_mask);
  ASSERT_EQ(LIBC_NAMESPACE::sigaction(SIGUSR1, &action, nullptr), 0);

  ASSERT_EQ(LIBC_NAMESPACE::siginterrupt(SIGUSR1, 1), 0);
  struct sigaction got;
  ASSERT_EQ(LIBC_NAMESPACE::sigaction(SIGUSR1, nullptr, &got), 0);
  ASSERT_EQ(got.sa_flags & SA_RESTART, 0);

  ASSERT_EQ(LIBC_NAMESPACE::siginterrupt(SIGUSR1, 0), 0);
  ASSERT_EQ(LIBC_NAMESPACE::sigaction(SIGUSR1, nullptr, &got), 0);
  ASSERT_NE(got.sa_flags & SA_RESTART, 0);

  // What the signal does is otherwise left as it was.
  ASSERT_TRUE(got.sa_handler == &handler);
}

// A signal that does not exist is reported rather than acted on.
TEST_F(LlvmLibcSigInterruptTest, UnknownSignalFails) {
  ASSERT_EQ(LIBC_NAMESPACE::siginterrupt(-1, 1), -1);
  ASSERT_ERRNO_EQ(EINVAL);
}
