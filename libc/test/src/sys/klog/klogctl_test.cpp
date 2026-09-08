//===-- Unittests for klogctl ---------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "hdr/errno_macros.h"
#include "src/__support/libc_errno.h"
#include "src/sys/klog/klogctl.h"
#include "test/UnitTest/ErrnoCheckingTest.h"
#include "test/UnitTest/ErrnoSetterMatcher.h"
#include "test/UnitTest/Test.h"

using LlvmLibcKlogctlTest = LIBC_NAMESPACE::testing::ErrnoCheckingTest;

// Asking how much the kernel ring buffer holds reads nothing out of it, and
// a machine that allows unprivileged reads answers with a size. Where
// dmesg_restrict is set it is EPERM instead, and both are correct answers.
TEST_F(LlvmLibcKlogctlTest, AskingHowMuchIsBuffered) {
  constexpr int SYSLOG_ACTION_SIZE_UNREAD = 9;
  int result = LIBC_NAMESPACE::klogctl(SYSLOG_ACTION_SIZE_UNREAD, nullptr, 0);
  if (result < 0) {
    ASSERT_ERRNO_EQ(EPERM);
    LIBC_NAMESPACE::libc_errno = 0;
    return;
  }
  ASSERT_ERRNO_SUCCESS();
  ASSERT_GE(result, 0);
}

TEST_F(LlvmLibcKlogctlTest, ATypeThatIsNotOne) {
  // The kernel looks at the privilege before it looks at the type, so a
  // caller without it is told EPERM rather than which type it asked for.
  ASSERT_EQ(LIBC_NAMESPACE::klogctl(-1, nullptr, 0), -1);
  int err = static_cast<int>(LIBC_NAMESPACE::libc_errno);
  ASSERT_TRUE(err == EINVAL || err == EPERM);
  LIBC_NAMESPACE::libc_errno = 0;
}
