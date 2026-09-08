//===-- Unittests for waitid ----------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "hdr/errno_macros.h"
#include "hdr/signal_macros.h"
#include "hdr/types/siginfo_t.h"
#include "src/stdlib/_Exit.h"
#include "src/sys/wait/waitid.h"
#include "src/unistd/fork.h"
#include "test/UnitTest/ErrnoCheckingTest.h"
#include "test/UnitTest/ErrnoSetterMatcher.h"
#include "test/UnitTest/Test.h"

#include <sys/wait.h>

using LlvmLibcWaitidTest = LIBC_NAMESPACE::testing::ErrnoCheckingTest;
using LIBC_NAMESPACE::testing::ErrnoSetterMatcher::Fails;
using LIBC_NAMESPACE::testing::ErrnoSetterMatcher::Succeeds;

// waitid reports how the child ended in a siginfo rather than in a packed
// status, which is what separates it from the rest of the wait family.
TEST_F(LlvmLibcWaitidTest, ReportsHowAChildExited) {
  pid_t pid = LIBC_NAMESPACE::fork();
  ASSERT_GE(pid, 0);
  if (pid == 0)
    LIBC_NAMESPACE::_Exit(7);

  siginfo_t info = {};
  ASSERT_THAT(LIBC_NAMESPACE::waitid(P_PID, pid, &info, WEXITED), Succeeds(0));
  ASSERT_EQ(info.si_pid, pid);
  ASSERT_EQ(info.si_signo, SIGCHLD);
  ASSERT_EQ(info.si_code, static_cast<int>(CLD_EXITED));
  ASSERT_EQ(info.si_status, 7);
}

TEST_F(LlvmLibcWaitidTest, NoChildToWaitFor) {
  siginfo_t info = {};
  ASSERT_THAT(LIBC_NAMESPACE::waitid(P_PID, 1, &info, WEXITED), Fails(ECHILD));
}

TEST_F(LlvmLibcWaitidTest, OptionsThatSayNothingToWaitFor) {
  siginfo_t info = {};
  ASSERT_THAT(LIBC_NAMESPACE::waitid(P_ALL, 0, &info, 0), Fails(EINVAL));
}
