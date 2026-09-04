//===-- Unittests for eventfd ---------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "hdr/errno_macros.h"
#include "hdr/sys_eventfd_macros.h"
#include "hdr/types/eventfd_t.h"
#include "src/sys/eventfd/eventfd.h"
#include "src/sys/eventfd/eventfd_read.h"
#include "src/sys/eventfd/eventfd_write.h"
#include "src/unistd/close.h"
#include "test/UnitTest/ErrnoCheckingTest.h"
#include "test/UnitTest/ErrnoSetterMatcher.h"
#include "test/UnitTest/Test.h"

using namespace LIBC_NAMESPACE::testing::ErrnoSetterMatcher;
using LlvmLibcEventfdTest = LIBC_NAMESPACE::testing::ErrnoCheckingTest;

TEST_F(LlvmLibcEventfdTest, WritesAddUpAndAReadTakesThemAll) {
  int fd = LIBC_NAMESPACE::eventfd(0, EFD_NONBLOCK);
  ASSERT_GT(fd, 0);
  ASSERT_ERRNO_SUCCESS();

  ASSERT_THAT(LIBC_NAMESPACE::eventfd_write(fd, 7), Succeeds(0));
  ASSERT_THAT(LIBC_NAMESPACE::eventfd_write(fd, 5), Succeeds(0));

  eventfd_t value = 0;
  ASSERT_THAT(LIBC_NAMESPACE::eventfd_read(fd, &value), Succeeds(0));
  EXPECT_EQ(value, eventfd_t(12));

  ASSERT_THAT(LIBC_NAMESPACE::close(fd), Succeeds(0));
}

TEST_F(LlvmLibcEventfdTest, AnEmptyCounterHasNothingToRead) {
  int fd = LIBC_NAMESPACE::eventfd(0, EFD_NONBLOCK);
  ASSERT_GT(fd, 0);

  eventfd_t value = 0;
  // Without EFD_NONBLOCK this would wait, which is the whole point of the
  // descriptor.
  EXPECT_EQ(LIBC_NAMESPACE::eventfd_read(fd, &value), -1);
  ASSERT_ERRNO_EQ(EAGAIN);

  ASSERT_THAT(LIBC_NAMESPACE::close(fd), Succeeds(0));
}

TEST_F(LlvmLibcEventfdTest, TheInitialValueIsThere) {
  int fd = LIBC_NAMESPACE::eventfd(42, EFD_NONBLOCK);
  ASSERT_GT(fd, 0);

  eventfd_t value = 0;
  ASSERT_THAT(LIBC_NAMESPACE::eventfd_read(fd, &value), Succeeds(0));
  EXPECT_EQ(value, eventfd_t(42));

  ASSERT_THAT(LIBC_NAMESPACE::close(fd), Succeeds(0));
}

TEST_F(LlvmLibcEventfdTest, SemaphoreModeTakesOneAtATime) {
  int fd = LIBC_NAMESPACE::eventfd(3, EFD_NONBLOCK | EFD_SEMAPHORE);
  ASSERT_GT(fd, 0);

  eventfd_t value = 0;
  // Each read takes one rather than the whole count.
  ASSERT_THAT(LIBC_NAMESPACE::eventfd_read(fd, &value), Succeeds(0));
  EXPECT_EQ(value, eventfd_t(1));
  ASSERT_THAT(LIBC_NAMESPACE::eventfd_read(fd, &value), Succeeds(0));
  EXPECT_EQ(value, eventfd_t(1));

  ASSERT_THAT(LIBC_NAMESPACE::close(fd), Succeeds(0));
}

TEST_F(LlvmLibcEventfdTest, BadDescriptor) {
  eventfd_t value = 0;
  EXPECT_EQ(LIBC_NAMESPACE::eventfd_read(-1, &value), -1);
  ASSERT_ERRNO_EQ(EBADF);
  EXPECT_EQ(LIBC_NAMESPACE::eventfd_write(-1, 1), -1);
  ASSERT_ERRNO_EQ(EBADF);
}
