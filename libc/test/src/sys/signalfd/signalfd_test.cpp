//===-- Unittests for signalfd --------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "hdr/errno_macros.h"
#include "hdr/signal_macros.h"
#include "hdr/types/sigset_t.h"
#include "src/signal/kill.h"
#include "src/signal/sigaddset.h"
#include "src/signal/sigemptyset.h"
#include "src/signal/sigprocmask.h"
#include "src/sys/signalfd/signalfd.h"
#include "src/unistd/close.h"
#include "src/unistd/getpid.h"
#include "src/unistd/read.h"
#include "test/UnitTest/ErrnoCheckingTest.h"
#include "test/UnitTest/ErrnoSetterMatcher.h"
#include "test/UnitTest/Test.h"

#include <sys/signalfd.h>

using LlvmLibcSignalfdTest = LIBC_NAMESPACE::testing::ErrnoCheckingTest;
using LIBC_NAMESPACE::testing::ErrnoSetterMatcher::Fails;
using LIBC_NAMESPACE::testing::ErrnoSetterMatcher::Succeeds;

// A signal read through a descriptor has to be blocked, or it is delivered
// the ordinary way instead.
TEST_F(LlvmLibcSignalfdTest, ReadsASignalThatWasSent) {
  sigset_t mask;
  ASSERT_EQ(LIBC_NAMESPACE::sigemptyset(&mask), 0);
  ASSERT_EQ(LIBC_NAMESPACE::sigaddset(&mask, SIGUSR1), 0);

  sigset_t before;
  ASSERT_THAT(LIBC_NAMESPACE::sigprocmask(SIG_BLOCK, &mask, &before),
              Succeeds(0));

  int fd = LIBC_NAMESPACE::signalfd(-1, &mask, 0);
  ASSERT_ERRNO_SUCCESS();
  ASSERT_GT(fd, 0);

  ASSERT_THAT(LIBC_NAMESPACE::kill(LIBC_NAMESPACE::getpid(), SIGUSR1),
              Succeeds(0));

  struct signalfd_siginfo info = {};
  ASSERT_THAT(LIBC_NAMESPACE::read(fd, &info, sizeof(info)),
              Succeeds<ssize_t>(sizeof(info)));
  ASSERT_EQ(static_cast<int>(info.ssi_signo), SIGUSR1);

  ASSERT_THAT(LIBC_NAMESPACE::close(fd), Succeeds(0));
  ASSERT_THAT(LIBC_NAMESPACE::sigprocmask(SIG_SETMASK, &before, nullptr),
              Succeeds(0));
}

TEST_F(LlvmLibcSignalfdTest, DescriptorThatIsNotOne) {
  sigset_t mask;
  ASSERT_EQ(LIBC_NAMESPACE::sigemptyset(&mask), 0);
  ASSERT_THAT(LIBC_NAMESPACE::signalfd(-2, &mask, 0), Fails(EBADF));
}
