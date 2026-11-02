//===-- Unittests for wait3 -----------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Unittests for wait3.
///
//===----------------------------------------------------------------------===//

#include "src/sys/wait/wait3.h"
#include "src/unistd/_exit.h"
#include "src/unistd/fork.h"
#include "test/UnitTest/ErrnoCheckingTest.h"
#include "test/UnitTest/ErrnoSetterMatcher.h"
#include "test/UnitTest/Test.h"

#include <sys/wait.h>

using namespace LIBC_NAMESPACE::testing::ErrnoSetterMatcher;
using LlvmLibcWait3Test = LIBC_NAMESPACE::testing::ErrnoCheckingTest;

TEST_F(LlvmLibcWait3Test, NoChildren) {
  int status;
  ASSERT_THAT(LIBC_NAMESPACE::wait3(&status, WNOHANG, nullptr), Fails(ECHILD));
}

TEST_F(LlvmLibcWait3Test, ReapsAnyChild) {
  pid_t pid = LIBC_NAMESPACE::fork();
  if (pid == 0)
    LIBC_NAMESPACE::_exit(7);
  ASSERT_GT(pid, 0);
  int status = 0;
  struct rusage usage;
  ASSERT_THAT(LIBC_NAMESPACE::wait3(&status, 0, &usage), Succeeds(pid));
  ASSERT_TRUE(WIFEXITED(status));
  ASSERT_EQ(WEXITSTATUS(status), 7);
}
