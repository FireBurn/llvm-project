//===-- Unittests for the timerfd calls -----------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "hdr/errno_macros.h"
#include "hdr/time_macros.h"
#include "hdr/types/struct_itimerspec.h"
#include "src/sys/timerfd/timerfd_create.h"
#include "src/sys/timerfd/timerfd_gettime.h"
#include "src/sys/timerfd/timerfd_settime.h"
#include "src/unistd/close.h"
#include "src/unistd/read.h"
#include "test/UnitTest/ErrnoCheckingTest.h"
#include "test/UnitTest/ErrnoSetterMatcher.h"
#include "test/UnitTest/Test.h"

#include <sys/timerfd.h>

using LlvmLibcTimerfdTest = LIBC_NAMESPACE::testing::ErrnoCheckingTest;
using LIBC_NAMESPACE::testing::ErrnoSetterMatcher::Fails;
using LIBC_NAMESPACE::testing::ErrnoSetterMatcher::Succeeds;

TEST_F(LlvmLibcTimerfdTest, ATimerThatGoesOffOnce) {
  int fd = LIBC_NAMESPACE::timerfd_create(CLOCK_MONOTONIC, 0);
  ASSERT_ERRNO_SUCCESS();
  ASSERT_GT(fd, 0);

  // A timer that has not been armed reads as zero.
  struct itimerspec current = {};
  ASSERT_THAT(LIBC_NAMESPACE::timerfd_gettime(fd, &current), Succeeds(0));
  ASSERT_EQ(current.it_value.tv_sec, static_cast<time_t>(0));
  ASSERT_EQ(current.it_value.tv_nsec, 0L);

  struct itimerspec spec = {};
  spec.it_value.tv_nsec = 1000000; // A millisecond from now.
  struct itimerspec previous = {};
  ASSERT_THAT(LIBC_NAMESPACE::timerfd_settime(fd, 0, &spec, &previous),
              Succeeds(0));
  ASSERT_EQ(previous.it_value.tv_sec, static_cast<time_t>(0));

  // Reading blocks until it goes off, and reports how many times it did.
  unsigned long long count = 0;
  ASSERT_THAT(LIBC_NAMESPACE::read(fd, &count, sizeof(count)),
              Succeeds<ssize_t>(sizeof(count)));
  ASSERT_GE(count, 1ULL);

  // Having gone off once and not been set to repeat, it is disarmed again.
  ASSERT_THAT(LIBC_NAMESPACE::timerfd_gettime(fd, &current), Succeeds(0));
  ASSERT_EQ(current.it_value.tv_sec, static_cast<time_t>(0));
  ASSERT_EQ(current.it_value.tv_nsec, 0L);

  ASSERT_THAT(LIBC_NAMESPACE::close(fd), Succeeds(0));
}

TEST_F(LlvmLibcTimerfdTest, AClockThatIsNotOne) {
  ASSERT_THAT(LIBC_NAMESPACE::timerfd_create(-1, 0), Fails(EINVAL));
}

TEST_F(LlvmLibcTimerfdTest, DescriptorThatIsNotOne) {
  struct itimerspec spec = {};
  ASSERT_THAT(LIBC_NAMESPACE::timerfd_gettime(-1, &spec), Fails(EBADF));
  ASSERT_THAT(LIBC_NAMESPACE::timerfd_settime(-1, 0, &spec, nullptr),
              Fails(EBADF));
}
