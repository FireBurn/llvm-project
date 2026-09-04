//===-- Unittests for pselect ---------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/signal/sigaction.h"
#include "src/signal/sigaddset.h"
#include "src/signal/sigemptyset.h"
#include "src/sys/select/pselect.h"
#include "src/sys/select/select.h"
#include "src/unistd/alarm.h"
#include "src/unistd/close.h"
#include "src/unistd/pipe.h"
#include "src/unistd/write.h"
#include "test/UnitTest/ErrnoCheckingTest.h"
#include "test/UnitTest/ErrnoSetterMatcher.h"
#include "test/UnitTest/Test.h"

#include <sys/select.h>

using namespace LIBC_NAMESPACE::testing::ErrnoSetterMatcher;
using LlvmLibcPSelectTest = LIBC_NAMESPACE::testing::ErrnoCheckingTest;

namespace {

void noop_handler(int) {}

// Without a handler the default action for SIGALRM ends the process, so the
// tests below could never observe the wait being cut short.
void catch_alarm() {
  struct sigaction action;
  action.sa_handler = noop_handler;
  action.sa_flags = 0;
  LIBC_NAMESPACE::sigemptyset(&action.sa_mask);
  LIBC_NAMESPACE::sigaction(SIGALRM, &action, nullptr);
}

} // anonymous namespace

TEST_F(LlvmLibcPSelectTest, ZeroTimeoutDoesNotWait) {
  int fds[2];
  ASSERT_THAT(LIBC_NAMESPACE::pipe(fds), Succeeds(0));

  fd_set set;
  FD_ZERO(&set);
  FD_SET(fds[0], &set);
  struct timespec zero{0, 0};
  // Nothing has been written, so the read end is not ready.
  ASSERT_THAT(LIBC_NAMESPACE::pselect(fds[0] + 1, &set, nullptr, nullptr, &zero,
                                      nullptr),
              Succeeds(0));
  EXPECT_EQ(0, FD_ISSET(fds[0], &set));

  char c = 'x';
  ASSERT_THAT(LIBC_NAMESPACE::write(fds[1], &c, 1), Succeeds(ssize_t(1)));

  FD_ZERO(&set);
  FD_SET(fds[0], &set);
  ASSERT_THAT(LIBC_NAMESPACE::pselect(fds[0] + 1, &set, nullptr, nullptr, &zero,
                                      nullptr),
              Succeeds(1));
  EXPECT_EQ(1, FD_ISSET(fds[0], &set));

  ASSERT_THAT(LIBC_NAMESPACE::close(fds[0]), Succeeds(0));
  ASSERT_THAT(LIBC_NAMESPACE::close(fds[1]), Succeeds(0));
}

TEST_F(LlvmLibcPSelectTest, TimeoutIsNotWrittenBack) {
  int fds[2];
  ASSERT_THAT(LIBC_NAMESPACE::pipe(fds), Succeeds(0));

  fd_set set;
  FD_ZERO(&set);
  FD_SET(fds[0], &set);
  // The pselect6 syscall writes the time left back to its argument. POSIX
  // says pselect must leave the caller's timespec alone.
  struct timespec timeout{0, 1000000};
  ASSERT_THAT(LIBC_NAMESPACE::pselect(fds[0] + 1, &set, nullptr, nullptr,
                                      &timeout, nullptr),
              Succeeds(0));
  EXPECT_EQ(timeout.tv_sec, static_cast<decltype(timeout.tv_sec)>(0));
  EXPECT_EQ(timeout.tv_nsec, static_cast<decltype(timeout.tv_nsec)>(1000000));

  ASSERT_THAT(LIBC_NAMESPACE::close(fds[0]), Succeeds(0));
  ASSERT_THAT(LIBC_NAMESPACE::close(fds[1]), Succeeds(0));
}

TEST_F(LlvmLibcPSelectTest, NullTimeoutBlocks) {
  int fds[2];
  ASSERT_THAT(LIBC_NAMESPACE::pipe(fds), Succeeds(0));

  fd_set set;
  FD_ZERO(&set);
  FD_SET(fds[0], &set);
  // A null timeout waits for a descriptor rather than polling, so the only
  // thing that ends this call is the alarm.
  catch_alarm();
  ASSERT_EQ(LIBC_NAMESPACE::alarm(1), 0u);
  ASSERT_THAT(LIBC_NAMESPACE::pselect(fds[0] + 1, &set, nullptr, nullptr,
                                      nullptr, nullptr),
              Fails(EINTR, -1));

  ASSERT_THAT(LIBC_NAMESPACE::close(fds[0]), Succeeds(0));
  ASSERT_THAT(LIBC_NAMESPACE::close(fds[1]), Succeeds(0));
}

TEST_F(LlvmLibcPSelectTest, SelectNullTimeoutBlocks) {
  int fds[2];
  ASSERT_THAT(LIBC_NAMESPACE::pipe(fds), Succeeds(0));

  fd_set set;
  FD_ZERO(&set);
  FD_SET(fds[0], &set);
  catch_alarm();
  ASSERT_EQ(LIBC_NAMESPACE::alarm(1), 0u);
  ASSERT_THAT(
      LIBC_NAMESPACE::select(fds[0] + 1, &set, nullptr, nullptr, nullptr),
      Fails(EINTR, -1));

  ASSERT_THAT(LIBC_NAMESPACE::close(fds[0]), Succeeds(0));
  ASSERT_THAT(LIBC_NAMESPACE::close(fds[1]), Succeeds(0));
}

TEST_F(LlvmLibcPSelectTest, MaskBlocksTheAlarm) {
  int fds[2];
  ASSERT_THAT(LIBC_NAMESPACE::pipe(fds), Succeeds(0));

  sigset_t mask;
  ASSERT_THAT(LIBC_NAMESPACE::sigemptyset(&mask), Succeeds(0));
  ASSERT_THAT(LIBC_NAMESPACE::sigaddset(&mask, SIGALRM), Succeeds(0));

  fd_set set;
  FD_ZERO(&set);
  FD_SET(fds[0], &set);
  struct timespec timeout{1, 0};
  // With SIGALRM blocked for the duration of the call, the alarm cannot cut
  // the wait short, so this runs out the timeout instead of failing EINTR.
  catch_alarm();
  ASSERT_EQ(LIBC_NAMESPACE::alarm(1), 0u);
  ASSERT_THAT(LIBC_NAMESPACE::pselect(fds[0] + 1, &set, nullptr, nullptr,
                                      &timeout, &mask),
              Succeeds(0));

  ASSERT_THAT(LIBC_NAMESPACE::close(fds[0]), Succeeds(0));
  ASSERT_THAT(LIBC_NAMESPACE::close(fds[1]), Succeeds(0));
}
