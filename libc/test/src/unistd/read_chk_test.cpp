//===-- Unittests for read and write --------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for __read_chk.
///
//===----------------------------------------------------------------------===//

#include "hdr/signal_macros.h"
#include "src/unistd/close.h"
#include "src/unistd/pipe.h"
#include "src/unistd/read_chk.h"
#include "src/unistd/write.h"
#include "test/UnitTest/ErrnoCheckingTest.h"
#include "test/UnitTest/ErrnoSetterMatcher.h"
#include "test/UnitTest/Test.h"

using LlvmLibcReadChkTest = LIBC_NAMESPACE::testing::ErrnoCheckingTest;

TEST_F(LlvmLibcReadChkTest, ReadsWhatFits) {
  using LIBC_NAMESPACE::testing::ErrnoSetterMatcher::Succeeds;
  int pipefd[2];
  ASSERT_THAT(LIBC_NAMESPACE::pipe(pipefd), Succeeds(0));
  ASSERT_THAT(LIBC_NAMESPACE::write(pipefd[1], "abc", 3), Succeeds(3));
  char buf[10];
  ASSERT_THAT(LIBC_NAMESPACE::__read_chk(pipefd[0], buf, 3, sizeof(buf)),
              Succeeds(3));
  EXPECT_EQ(buf[0], 'a');
  EXPECT_EQ(buf[2], 'c');
  ASSERT_THAT(LIBC_NAMESPACE::close(pipefd[0]), Succeeds(0));
  ASSERT_THAT(LIBC_NAMESPACE::close(pipefd[1]), Succeeds(0));
}

TEST_F(LlvmLibcReadChkTest, AbortsWhenTheCountExceedsTheBuffer) {
  // Standard error is closed in the child so the message is not printed.
  EXPECT_DEATH(
      [] {
        char buf[10];
        LIBC_NAMESPACE::close(2);
        LIBC_NAMESPACE::__read_chk(-1, buf, 32, sizeof(buf));
      },
      WITH_SIGNAL(SIGABRT));
}
