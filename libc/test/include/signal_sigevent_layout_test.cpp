//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for the layout of struct sigevent
///
//===----------------------------------------------------------------------===//

#include "include/signal.h"
#include "test/UnitTest/Test.h"

static void handler(union sigval) {}

TEST(LlvmLibcSignalSigeventLayoutTest, MembersAreReachableByTheirNames) {
  struct sigevent event;
  __builtin_memset(&event, 0, sizeof(event));
  event.sigev_signo = SIGUSR1;
  event.sigev_notify = SIGEV_THREAD;
  event.sigev_notify_function = handler;
  event.sigev_notify_attributes = nullptr;
  EXPECT_EQ(event.sigev_signo, SIGUSR1);
  EXPECT_EQ(event.sigev_notify, int(SIGEV_THREAD));
  EXPECT_TRUE(event.sigev_notify_function == handler);
  event.sigev_notify_thread_id = 5;
  EXPECT_EQ(event.sigev_notify_thread_id, 5);
}

// The kernel reads the structure as it lies on x86_64: two ints, then a union
// padded to make the whole 64 bytes.
TEST(LlvmLibcSignalSigeventLayoutTest, SizeAndOffsetsMatchTheKernel) {
#if defined(__x86_64__) && defined(__linux__)
  EXPECT_EQ(sizeof(struct sigevent), size_t(64));
  EXPECT_EQ(__builtin_offsetof(struct sigevent, sigev_signo), size_t(8));
  EXPECT_EQ(__builtin_offsetof(struct sigevent, sigev_notify), size_t(12));
#endif
}
