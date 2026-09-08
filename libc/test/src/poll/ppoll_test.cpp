//===-- Unittests for ppoll -----------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "hdr/errno_macros.h"
#include "hdr/signal_macros.h"
#include "hdr/types/sigset_t.h"
#include "hdr/types/struct_pollfd.h"
#include "hdr/types/struct_timespec.h"
#include "src/poll/ppoll.h"
#include "src/signal/sigemptyset.h"
#include "src/unistd/close.h"
#include "src/unistd/pipe.h"
#include "src/unistd/write.h"
#include "test/UnitTest/ErrnoCheckingTest.h"
#include "test/UnitTest/ErrnoSetterMatcher.h"
#include "test/UnitTest/Test.h"

#include <poll.h>

using LlvmLibcPpollTest = LIBC_NAMESPACE::testing::ErrnoCheckingTest;
using LIBC_NAMESPACE::testing::ErrnoSetterMatcher::Fails;
using LIBC_NAMESPACE::testing::ErrnoSetterMatcher::Succeeds;

// ppoll is poll with the wait stated in nanoseconds and a signal mask
// applied for as long as it waits.
TEST_F(LlvmLibcPpollTest, ReportsADescriptorThatIsReady) {
  int fds[2];
  ASSERT_THAT(LIBC_NAMESPACE::pipe(fds), Succeeds(0));

  struct pollfd watch;
  watch.fd = fds[0];
  watch.events = POLLIN;
  watch.revents = 0;

  // Nothing has been written, so a wait of no time at all finds nothing.
  struct timespec none = {};
  ASSERT_THAT(LIBC_NAMESPACE::ppoll(&watch, 1, &none, nullptr), Succeeds(0));
  ASSERT_EQ(int(watch.revents), 0);

  ASSERT_THAT(LIBC_NAMESPACE::write(fds[1], "x", 1), Succeeds<ssize_t>(1));

  sigset_t mask;
  ASSERT_EQ(LIBC_NAMESPACE::sigemptyset(&mask), 0);
  ASSERT_THAT(LIBC_NAMESPACE::ppoll(&watch, 1, &none, &mask), Succeeds(1));
  ASSERT_EQ(watch.revents & POLLIN, int(POLLIN));

  ASSERT_THAT(LIBC_NAMESPACE::close(fds[0]), Succeeds(0));
  ASSERT_THAT(LIBC_NAMESPACE::close(fds[1]), Succeeds(0));
}

// A descriptor that was closed is reported through the entry rather than by
// failing the call. A negative one is not a descriptor at all and is passed
// over, which is how a caller leaves a hole in the array.
TEST_F(LlvmLibcPpollTest, DescriptorThatIsNotOne) {
  int fds[2];
  ASSERT_THAT(LIBC_NAMESPACE::pipe(fds), Succeeds(0));
  ASSERT_THAT(LIBC_NAMESPACE::close(fds[0]), Succeeds(0));
  ASSERT_THAT(LIBC_NAMESPACE::close(fds[1]), Succeeds(0));

  struct pollfd watch;
  watch.fd = fds[0];
  watch.events = POLLIN;
  watch.revents = 0;

  struct timespec none = {};
  ASSERT_THAT(LIBC_NAMESPACE::ppoll(&watch, 1, &none, nullptr), Succeeds(1));
  ASSERT_EQ(watch.revents & POLLNVAL, int(POLLNVAL));

  watch.fd = -1;
  watch.revents = 0;
  ASSERT_THAT(LIBC_NAMESPACE::ppoll(&watch, 1, &none, nullptr), Succeeds(0));
  ASSERT_EQ(int(watch.revents), 0);
}

TEST_F(LlvmLibcPpollTest, ATimeThatIsNotOne) {
  struct pollfd watch;
  watch.fd = 0;
  watch.events = POLLIN;
  watch.revents = 0;

  struct timespec bad = {};
  bad.tv_nsec = 2000000000; // More than a second's worth of nanoseconds.
  ASSERT_THAT(LIBC_NAMESPACE::ppoll(&watch, 1, &bad, nullptr), Fails(EINVAL));
}
