//===-- Unittests for the POSIX message queues ----------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "hdr/errno_macros.h"
#include "hdr/fcntl_macros.h"
#include "hdr/sys_stat_macros.h"
#include "hdr/time_macros.h"
#include "hdr/types/mqd_t.h"
#include "hdr/types/struct_mq_attr.h"
#include "hdr/types/struct_timespec.h"
#include "src/__support/libc_errno.h"
#include "src/mqueue/mq_close.h"
#include "src/mqueue/mq_getattr.h"
#include "src/mqueue/mq_notify.h"
#include "src/mqueue/mq_open.h"
#include "src/mqueue/mq_receive.h"
#include "src/mqueue/mq_send.h"
#include "src/mqueue/mq_setattr.h"
#include "src/mqueue/mq_timedreceive.h"
#include "src/mqueue/mq_timedsend.h"
#include "src/mqueue/mq_unlink.h"
#include "src/string/memcmp.h"
#include "src/time/clock_gettime.h"
#include "test/UnitTest/ErrnoCheckingTest.h"
#include "test/UnitTest/Test.h"

namespace {

constexpr const char NAME[] = "/llvm_libc_mqueue_test";
constexpr const char MESSAGE[] = "a message";
constexpr size_t MESSAGE_SIZE = sizeof(MESSAGE);

// A queue can only be made where the kernel offers them, which is a mount of
// mqueue at /dev/mqueue. Where there is none the call fails and the test has
// nothing to say rather than something to fail.
bool have_queues() {
  struct mq_attr attr = {};
  attr.mq_maxmsg = 4;
  attr.mq_msgsize = 64;
  mqd_t q =
      LIBC_NAMESPACE::mq_open(NAME, O_CREAT | O_EXCL | O_RDWR, S_IRWXU, &attr);
  if (q == static_cast<mqd_t>(-1)) {
    LIBC_NAMESPACE::libc_errno = 0;
    return false;
  }
  LIBC_NAMESPACE::mq_close(q);
  LIBC_NAMESPACE::mq_unlink(NAME);
  LIBC_NAMESPACE::libc_errno = 0;
  return true;
}

mqd_t make_queue() {
  struct mq_attr attr = {};
  attr.mq_maxmsg = 4;
  attr.mq_msgsize = 64;
  return LIBC_NAMESPACE::mq_open(NAME, O_CREAT | O_EXCL | O_RDWR, S_IRWXU,
                                 &attr);
}

} // anonymous namespace

using LlvmLibcMqueueTest = LIBC_NAMESPACE::testing::ErrnoCheckingTest;

TEST_F(LlvmLibcMqueueTest, SendAndReceive) {
  if (!have_queues())
    return;

  mqd_t q = make_queue();
  ASSERT_NE(q, static_cast<mqd_t>(-1));

  ASSERT_EQ(LIBC_NAMESPACE::mq_send(q, MESSAGE, MESSAGE_SIZE, 3), 0);

  char buf[64];
  unsigned int prio = 0;
  ASSERT_EQ(LIBC_NAMESPACE::mq_receive(q, buf, sizeof(buf), &prio),
            static_cast<ssize_t>(MESSAGE_SIZE));
  ASSERT_EQ(prio, 3U);
  ASSERT_EQ(LIBC_NAMESPACE::memcmp(buf, MESSAGE, MESSAGE_SIZE), 0);

  ASSERT_EQ(LIBC_NAMESPACE::mq_close(q), 0);
  ASSERT_EQ(LIBC_NAMESPACE::mq_unlink(NAME), 0);
}

TEST_F(LlvmLibcMqueueTest, Attributes) {
  if (!have_queues())
    return;

  mqd_t q = make_queue();
  ASSERT_NE(q, static_cast<mqd_t>(-1));

  struct mq_attr attr = {};
  ASSERT_EQ(LIBC_NAMESPACE::mq_getattr(q, &attr), 0);
  ASSERT_EQ(attr.mq_maxmsg, 4L);
  ASSERT_EQ(attr.mq_msgsize, 64L);
  ASSERT_EQ(attr.mq_curmsgs, 0L);

  // Only the flags may be changed, and the old ones are handed back.
  struct mq_attr set = attr;
  set.mq_flags = O_NONBLOCK;
  struct mq_attr old = {};
  ASSERT_EQ(LIBC_NAMESPACE::mq_setattr(q, &set, &old), 0);
  ASSERT_EQ(old.mq_flags, 0L);

  ASSERT_EQ(LIBC_NAMESPACE::mq_getattr(q, &attr), 0);
  ASSERT_EQ(attr.mq_flags, static_cast<long>(O_NONBLOCK));

  // An empty queue that is not to block reports that there is nothing there.
  char buf[64];
  ASSERT_EQ(LIBC_NAMESPACE::mq_receive(q, buf, sizeof(buf), nullptr),
            static_cast<ssize_t>(-1));
  ASSERT_ERRNO_EQ(EAGAIN);
  LIBC_NAMESPACE::libc_errno = 0;

  ASSERT_EQ(LIBC_NAMESPACE::mq_close(q), 0);
  ASSERT_EQ(LIBC_NAMESPACE::mq_unlink(NAME), 0);
}

TEST_F(LlvmLibcMqueueTest, WaitingWithADeadline) {
  if (!have_queues())
    return;

  mqd_t q = make_queue();
  ASSERT_NE(q, static_cast<mqd_t>(-1));

  struct timespec deadline;
  ASSERT_EQ(LIBC_NAMESPACE::clock_gettime(CLOCK_REALTIME, &deadline), 0);
  deadline.tv_sec += 5;

  ASSERT_EQ(
      LIBC_NAMESPACE::mq_timedsend(q, MESSAGE, MESSAGE_SIZE, 1, &deadline), 0);

  char buf[64];
  unsigned int prio = 0;
  ASSERT_EQ(
      LIBC_NAMESPACE::mq_timedreceive(q, buf, sizeof(buf), &prio, &deadline),
      static_cast<ssize_t>(MESSAGE_SIZE));
  ASSERT_EQ(prio, 1U);

  // Nothing more is coming, so the deadline is what ends the wait.
  struct timespec soon;
  ASSERT_EQ(LIBC_NAMESPACE::clock_gettime(CLOCK_REALTIME, &soon), 0);
  ASSERT_EQ(
      LIBC_NAMESPACE::mq_timedreceive(q, buf, sizeof(buf), nullptr, &soon),
      static_cast<ssize_t>(-1));
  ASSERT_ERRNO_EQ(ETIMEDOUT);
  LIBC_NAMESPACE::libc_errno = 0;

  ASSERT_EQ(LIBC_NAMESPACE::mq_close(q), 0);
  ASSERT_EQ(LIBC_NAMESPACE::mq_unlink(NAME), 0);
}

TEST_F(LlvmLibcMqueueTest, AskingToBeToldAndTakingItBack) {
  if (!have_queues())
    return;

  mqd_t q = make_queue();
  ASSERT_NE(q, static_cast<mqd_t>(-1));

  // A null event withdraws a registration; there is none, which is allowed.
  ASSERT_EQ(LIBC_NAMESPACE::mq_notify(q, nullptr), 0);

  ASSERT_EQ(LIBC_NAMESPACE::mq_close(q), 0);
  ASSERT_EQ(LIBC_NAMESPACE::mq_unlink(NAME), 0);
}

TEST_F(LlvmLibcMqueueTest, NameThatIsNotThere) {
  if (!have_queues())
    return;

  ASSERT_EQ(LIBC_NAMESPACE::mq_open("/llvm_libc_absent", O_RDONLY),
            static_cast<mqd_t>(-1));
  ASSERT_ERRNO_EQ(ENOENT);
  LIBC_NAMESPACE::libc_errno = 0;

  ASSERT_EQ(LIBC_NAMESPACE::mq_unlink("/llvm_libc_absent"), -1);
  ASSERT_ERRNO_EQ(ENOENT);
  LIBC_NAMESPACE::libc_errno = 0;
}

TEST_F(LlvmLibcMqueueTest, DescriptorThatIsNotOne) {
  char buf[8];
  struct mq_attr attr = {};
  ASSERT_EQ(LIBC_NAMESPACE::mq_getattr(static_cast<mqd_t>(-1), &attr), -1);
  ASSERT_ERRNO_EQ(EBADF);
  LIBC_NAMESPACE::libc_errno = 0;

  ASSERT_EQ(LIBC_NAMESPACE::mq_receive(static_cast<mqd_t>(-1), buf, sizeof(buf),
                                       nullptr),
            static_cast<ssize_t>(-1));
  ASSERT_ERRNO_EQ(EBADF);
  LIBC_NAMESPACE::libc_errno = 0;

  ASSERT_EQ(LIBC_NAMESPACE::mq_close(static_cast<mqd_t>(-1)), -1);
  ASSERT_ERRNO_EQ(EBADF);
  LIBC_NAMESPACE::libc_errno = 0;
}
